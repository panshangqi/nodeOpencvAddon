
#include <node.h>
#include <v8.h>
#include <uv.h>
#include <windows.h>
#include <string.h>
#include <string>
#include <iostream>
#include <thread>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>



using namespace v8;
// 传入了两个参数，args[0] 字符串，args[1] 回调函数s
struct SImageData {
	int status = -1;
	int width = 0;
	int height = 0;
	int channels = 0;
	int size = 0;
	int step = 0;
	std::vector<int> data;
	std::string msg; //msg
};


void hello(const FunctionCallbackInfo<Value>& args) {
  // 使用 HandleScope 来管理生命周期s
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // 判断参数格式和格式s
  if (args.Length() < 2 || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }

  // callback, 使用Cast方法来转换s
  Local<Function> callback = Local<Function>::Cast(args[1]);
  Local<Value> argv[1] = {
    // 拼接String
    String::Concat(Local<String>::Cast(args[0]), String::NewFromUtf8(isolate, " world"))
  };
  // 调用回调, 参数: 当前上下文，参数个数，参数列表s
  callback->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}
//三个参数  图片路径，目标图片宽度， 返回值： 图像参数，数据流s
void getImageData(const FunctionCallbackInfo<Value>& args){
    Isolate* isolate = args.GetIsolate();

    SImageData sData;
    // 参数长度判断s
    /*
    if (args.Length() < 3) {
     isolate->ThrowException(Exception::TypeError(
         String::NewFromUtf8(isolate, "Wrong params of arguments")));
         return ;
    }
    */
    // js String 类型转换成 v8 String 类型s
    Local<String> filepath = Local<String>::Cast(args[0]);
    String::Utf8Value filePath(filepath);
    // js Number 类型转换成 v8 Number 类型s
    Local<Number> dstWidth = Local<Number>::Cast(args[1]);
    printf("%s %f\n", *filePath, dstWidth->NumberValue());


    cv::Mat srcImg = cv::imread(std::string(*filePath));
    cv::Mat img;
    int width = srcImg.cols;
    int height = srcImg.rows;
     //图片压缩s
    int r_width = width;
    int r_height = r_width * height / width;

    resize(srcImg, img, cv::Size(r_width, r_height), 0, 0, 3);
	// 获取图片流信息
	int channels = img.channels();
	uchar* buffer = img.data;
	width = img.cols;
	height = img.rows;
	//printf("width = %d\n;height = %d\n;channels = %d\n;step = %d\n",width, height, channels, step);
	int size = width * channels * height;
    sData.data.clear();
    /*
	for (int i = 0; i < size; i += channels) {
		int r = buffer[i];
		int g = buffer[i + 1];
		int b = buffer[i + 2];
		sData.data.push_back(r);
		sData.data.push_back(g);
		sData.data.push_back(b);
	}
	*/
	sData.status = 1;
	sData.width = img.cols;
	sData.height = img.rows;
	sData.size = size;
	sData.step = img.step;
	sData.channels = channels;

     //c++ -> js
    Local<Object> obj = Object::New(isolate);
    // Number type
    Local<Number> jsStatus = Number::New(isolate, sData.status);
    Local<Number> jsWidth = Number::New(isolate, sData.width);
    Local<Number> jsHeight = Number::New(isolate, sData.height);
    Local<Number> jsSize = Number::New(isolate, sData.size);
    Local<Number> jsStep = Number::New(isolate, sData.step);
    Local<Number> jsChannels = Number::New(isolate, sData.channels);

    obj->Set(String::NewFromUtf8(isolate, "status"), jsStatus);
    obj->Set(String::NewFromUtf8(isolate, "width"), jsWidth);
    obj->Set(String::NewFromUtf8(isolate, "height"), jsHeight);
    obj->Set(String::NewFromUtf8(isolate, "size"), jsSize);
    obj->Set(String::NewFromUtf8(isolate, "step"), jsStep);
    obj->Set(String::NewFromUtf8(isolate, "channels"), jsChannels);

    //args.GetReturnValue().Set(obj);
    // callback, 使用Cast方法来转换s
    Local<Function> callback = Local<Function>::Cast(args[2]);
    const unsigned argc = 1;
    Local<Value> argv[argc] = { obj };
    //cb->Call(Null(isolate), argc, argv);
      // 调用回调, 参数: 当前上下文，参数个数，参数列表s
    callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
}
//async
//在AddOn中，定义一个结构体在异步调用中传递数据s
struct LookupIpCountryBaton {
	uv_work_t work;              //libuv
	Persistent<Function> callback;    //javascript callback  <heap>

	int type;
};

void lookupIpCountryAsync(uv_work_t * work){
	LookupIpCountryBaton * baton = (LookupIpCountryBaton*)work->data;

	// block thread for 3 seconds
	Sleep(100);
	// save the result
	baton->type = 100002;
}

void lookupIpCountryCompleted(uv_work_t * work, int){
	LookupIpCountryBaton * baton = (LookupIpCountryBaton*)work->data;
    Isolate * isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);
	const unsigned argc = 1;
	Local<Value> argv[argc] = {String::NewFromUtf8(isolate, "HHEE")};
	Local<Function>::New(isolate, baton->callback)->Call(isolate->GetCurrentContext()->Global(), argc, argv);

	baton->callback.Reset();
	delete baton;

}
//导出方法首先保存回调函数，并验证和解析传入参数s
// lookup country by ip
// 1st argument is ip address
// 2nd argument is the callback function
void lookupIpCountry(const FunctionCallbackInfo<Value>& args) {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
	HandleScope scope(isolate);
	if (args.Length() < 2) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong params of arguments")));
        return ;
	}

	LookupIpCountryBaton * baton = new LookupIpCountryBaton();
	baton->work.data = baton;
	baton->type = 200;

	//baton->callback = Persistent<Function>::New(Handle<Function>::Cast(args[1]));
	baton->callback.Reset(isolate, Local<Function>::Cast(args[1]));


	uv_queue_work( uv_default_loop(), &baton->work, lookupIpCountryAsync, lookupIpCountryCompleted);

	return ;
}

// 相当于在 exports 对象中添加 { hello: hello }
void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "hello", hello);
  NODE_SET_METHOD(exports, "getImageData", getImageData);
  NODE_SET_METHOD(exports, "delay", lookupIpCountry);
}

// 将 export 对象暴露出去s
// 原型 `NODE_MODULE(module_name, Initialize)`s
NODE_MODULE(opencv_image, init);
