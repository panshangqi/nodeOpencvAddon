#include <node.h>
#include <v8.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

using namespace v8;
// 传入了两个参数，args[0] 字符串，args[1] 回调函数

void hello(const FunctionCallbackInfo<Value>& args) {
  // 使用 HandleScope 来管理生命周期
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  // 判断参数格式和格式
  if (args.Length() < 2 || !args[0]->IsString()) {
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Wrong arguments")));
    return;
  }

  // callback, 使用Cast方法来转换
  Local<Function> callback = Local<Function>::Cast(args[1]);
  Local<Value> argv[1] = {
    // 拼接String
    String::Concat(Local<String>::Cast(args[0]), String::NewFromUtf8(isolate, " world"))
  };
  // 调用回调, 参数: 当前上下文，参数个数，参数列表
  callback->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}
//三个参数  图片路径，目标图片宽度， 返回值： 图像参数，数据流
void getImageData(const FunctionCallbackInfo<Value>& args){
     Isolate* isolate = args.GetIsolate();
     // 参数长度判断
     if (args.Length() < 2) {
         isolate->ThrowException(Exception::TypeError(
             String::NewFromUtf8(isolate, "Wrong params of arguments")));
         return;
     }
     // js String 类型转换成 v8 String 类型
     Local<String> filepath = Local<String>::Cast(args[0]);
     String::Utf8Value filePath(filepath);
     // js Number 类型转换成 v8 Number 类型
     Local<Number> dstWidth = Local<Number>::Cast(args[1]);
     printf("%s %f\n", *filePath, dstWidth->NumberValue());

     //std::cout << filepath << std::endl;
     //cv::Mat srcImg = cv::imread(std::string(*filePath));
     // C: IplImage based
     IplImage* srcI = cvLoadImage(std::string(*filePath).c_str(), CV_LOAD_IMAGE_COLOR );
}

// 相当于在 exports 对象中添加 { hello: hello }
void init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "hello", hello);
  NODE_SET_METHOD(exports, "getImageData", getImageData);
}

// 将 export 对象暴露出去
// 原型 `NODE_MODULE(module_name, Initialize)`
NODE_MODULE(opencv_image, init);
