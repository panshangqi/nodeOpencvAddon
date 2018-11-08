{
    "targets": [
        {
            "target_name": "opencv_image",
            "sources": [ "./src/image_proc.cc" ],
            "include_dirs": [
                    "<(module_root_dir)/src/win64/include/opencv",
                    "<(module_root_dir)/src/win64/include/opencv2",
                    "<(module_root_dir)/src/win64/include/libvc",
                    "<(module_root_dir)/src/win64/include"
                  ],

            "libraries":[
                    "<(module_root_dir)/src/win64/lib/opencv_core249.lib",
                  "<(module_root_dir)/src/win64/lib/opencv_highgui249.lib",
                  "<(module_root_dir)/src/win64/lib/opencv_imgproc249.lib",
                  "<(module_root_dir)/src/win64/lib/libuv.lib"
                  ],
              "variables": {
                      "dll_files": ["opencv_core249.dll", "opencv_core249.dll", "opencv_core249.dll"]
                    },

            "cflags!": [ "-fno-exceptions" ],
            "cflags": [ "-std=c++11" ],
            "cflags_cc!": [ "-fno-exceptions" ]
        }
    ]
}