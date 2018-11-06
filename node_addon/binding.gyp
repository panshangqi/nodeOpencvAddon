{
    "targets": [
        {
            "target_name": "opencv_image",
            "sources": [ "./src/opencv_image.cc" ],
            "include_dirs": [
                    "<(module_root_dir)/src/win/include/opencv",
                    "<(module_root_dir)/src/win/include/opencv2",
                    "<(module_root_dir)/src/win/include"
                  ],

            "libraries":[
                    "<(module_root_dir)/src/win/lib/opencv_core249.lib",
                  "<(module_root_dir)/src/win/lib/opencv_highgui249.lib",
                  "<(module_root_dir)/src/win/lib/opencv_imgproc249.lib"
                  ],
            "cflags!": [ "-fno-exceptions" ],
            "cflags": [ "-std=c++11" ],
            "cflags_cc!": [ "-fno-exceptions" ]
        }
    ]
}