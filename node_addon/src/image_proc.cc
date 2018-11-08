
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
	char *data;
	std::string msg; //msg
};

//三个参数  图片路径，目标图片宽度， 返回值： 图像参数，数据流s
SImageData _get_image_data(std::string _filepath, int _dstWidth){

    SImageData sData;

    cv::Mat srcImg = cv::imread(_filepath);
    cv::Mat img;
    int width = srcImg.cols;
    int height = srcImg.rows;
     //图片压缩s
    int r_width = _dstWidth;
    int r_height = r_width * height / width;

    resize(srcImg, img, cv::Size(r_width, r_height), 0, 0, 3);
	// 获取图片流信息d
	int channels = img.channels();
	uchar* buffer = img.data;
	width = img.cols;
	height = img.rows;
	//printf("width = %d\n;height = %d\n;channels = %d\n;step = %d\n",width, height, channels, step);
	int size = width * channels * height;
    //sData.data = "[";
    char *_buffer = (char*)malloc(sizeof(char)*(size*4+4));
    int j = 0;

    int ascall[10];
    int int_base = 48;
    for(int i=0;i<10;i++){
        ascall[i] = int_base + i;
    }
    _buffer[j++] = 91;
	for (int i = 0; i < size; i += channels) {

		int r = buffer[i];
		int g = buffer[i + 1];
		int b = buffer[i + 2];

        char s[3];
        itoa(r, s, 10);
        if(i > 0){
            _buffer[j++] = 44;
        }
        for(int k=0;k<strlen(s)&k<3;k++){
            _buffer[j++] = (int)s[k];
        }

        itoa(g, s, 10);
        _buffer[j++] = 44;
        for(int k=0;k<strlen(s)&k<3;k++){
            _buffer[j++] = (int)s[k];
        }

        itoa(b, s, 10);
        _buffer[j++] = 44;
        for(int k=0;k<strlen(s)&k<3;k++){
            _buffer[j++] = (int)s[k];
        }

//		int r1 = r / 100;
//		int r3 = r % 10;
//		int r2 = ((r - r3)/10)%10;
//		if(i>0){
//		    _buffer[j++] = 44;
//		}
//        _buffer[j++] = ascall[r1];
//        _buffer[j++] = ascall[r2];
//        _buffer[j++] = ascall[r3];
//
//        int g1 = g / 100;
//        int g3 = g % 10;
//        int g2 = ((g - g3)/10)%10;
//        _buffer[j++] = 44;
//        _buffer[j++] = ascall[g1];
//        _buffer[j++] = ascall[g2];
//        _buffer[j++] = ascall[g3];
//
//        int b1 = b / 100;
//        int b3 = b % 10;
//        int b2 = ((b - b3)/10)%10;
//        _buffer[j++] = 44;
//        _buffer[j++] = ascall[b1];
//        _buffer[j++] = ascall[b2];
//        _buffer[j++] = ascall[b3];

	}
	_buffer[j++] = 93;
	_buffer[j] = '\0';
	//sData.data += "]";
    sData.data = _buffer;
	sData.status = 1;
	sData.width = img.cols;
	sData.height = img.rows;
	sData.size = size;
	sData.step = img.step;
	sData.channels = channels;
	srcImg.release();
	img.release();
    return sData;
}
//async
//在AddOn中，定义一个结构体在异步调用中传递数据s
struct WorkBaton {
	uv_work_t work;              //libuv
	Persistent<Function> callback;    //javascript callback  <heap>
	int type;
	cv::string filepath;
	int dstWidth;
	SImageData image;
};

void getImageDataAsync(uv_work_t * work){
	WorkBaton * baton = (WorkBaton*)work->data;

	// block t
	SImageData sData = _get_image_data(baton->filepath, baton->dstWidth);
    baton->image = sData;
	// save the result
}

void getImageDataCompleted(uv_work_t * work, int){
	WorkBaton * baton = (WorkBaton*)work->data;
    Isolate * isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);

	SImageData sData = baton->image;
	//c++ -> javascript
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
    obj->Set(String::NewFromUtf8(isolate, "data"), String::NewFromUtf8(isolate, sData.data));

    //args.GetReturnValue().Set(obj);
    // callback, 使用Cast方法来转换s
    const unsigned argc = 1;
    Local<Value> argv[argc] = { obj };
    Local<Function> callback = Local<Function>::New(isolate, baton->callback);
    // 调用回调, 参数: 当前上下文，参数个数，参数列表s
    callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);

	baton->callback.Reset();
	delete baton;
	if (NULL != baton->image.data)
    {
        free(baton->image.data);
        baton->image.data = NULL;
    }
}
//导出方法首先保存回调函数，并验证和解析传入参数s
// 1st argument is string : 'file' or 'url' <type>
// 1nd argument is string:  'c:\\' or 'http://'  <path>
// 3th argument is the callback function
void getImageData(const FunctionCallbackInfo<Value>& args) {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
	HandleScope scope(isolate);
	if (args.Length() < 2) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong params of arguments")));
        return ;
	}

    Local<Object> params = Local<Object>::Cast(args[0]);
    //url file params can't exist both
    bool hasUrl = params->HasRealNamedProperty(String::NewFromUtf8(isolate, "url"));
    bool hasFile = params->HasRealNamedProperty(String::NewFromUtf8(isolate, "file"));
    bool hasWidth = params->HasRealNamedProperty(String::NewFromUtf8(isolate, "width"));
    if(!hasWidth ){
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "lack param of width")));
        return ;
    }
    if(hasUrl && hasFile ){
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "url file params can't exist both")));
        return ;
    }


    WorkBaton *baton = new WorkBaton();
    baton->work.data = baton;

    if(hasUrl){
        Local<Value> value = params->Get(String::NewFromUtf8(isolate, "url"));
        String::Utf8Value sUrl(value);
        printf("url: %s\n", *sUrl);
        baton->type = 0;
        baton->filepath = *sUrl;

    }else if(hasFile){
        Local<Value> value = params->Get(String::NewFromUtf8(isolate, "file"));
        String::Utf8Value sFile(value);
        printf("file: %s\n", *sFile);
        baton->type = 1;
        baton->filepath = *sFile;

    }else{
        isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "need params url or file")));
        return ;
    }

    // js Number 类型转换成 v8 Number 类型s
    Local<Value> _dst_width = params->Get(String::NewFromUtf8(isolate, "width"));

    int dstWidth = _dst_width->NumberValue();
    printf("width: %d\n", dstWidth);
    baton->dstWidth = dstWidth;
	//baton->callback = Persistent<Function>::New(Handle<Function>::Cast(args[1]));
	baton->callback.Reset(isolate, Local<Function>::Cast(args[1]));

	uv_queue_work( uv_default_loop(), &baton->work, getImageDataAsync, getImageDataCompleted);

	return ;
}

// 相当于在 exports 对象中添加 { hello: hello }
void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "getImageData", getImageData);
}

// 将 export 对象暴露出去s
// 原型 `NODE_MODULE(module_name, Initialize)`s
NODE_MODULE(opencv_image, init);
