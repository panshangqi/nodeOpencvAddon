console.log('start copy')

var fs = require('fs')
var path = require('path')
var client_root = path.resolve(__dirname,'./');
console.log(client_root);

function copy(src, dst) {
    fs.writeFileSync(dst, fs.readFileSync(src));
}

var src_dir = path.join(client_root,'node_addon/src/win64/bin');;
var dst_dir = path.join(client_root,'node_addon/build/Release');
var files = [
    'opencv_imgproc249.dll',
    'opencv_highgui249.dll',
    'opencv_core249.dll'
]
//复制依赖文件
for(var i=0;i<files.length;i++){
    var _src = path.join(src_dir, files[i]);
    var _dst = path.join(dst_dir, files[i]);
    copy(_src, _dst);
    console.log('copy [' + _src + '] ===> [' + _dst + ']');
}


console.log('complete..');





