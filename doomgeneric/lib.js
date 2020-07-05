var ctx;
var width;
var height;
var screen_buffer_ptr;

mergeInto(LibraryManager.library, {
  init_js: function(resX, resY, ptr) {
	canvas = Module['canvas']
	canvas.width = resX;
	canvas.height = resY;
	ctx = canvas.getContext('2d');
	width = resX;
	height = resY;
	screen_buffer_ptr = ptr;
  },
  draw_js: function() {
	var imageData = ctx.getImageData(0, 0, width, height);
	var screen_buffer = Module.HEAPU8.slice(screen_buffer_ptr, screen_buffer_ptr + width * height * 4)
    imageData.data.set(screen_buffer);
	ctx.putImageData(imageData, 0, 0);
	//console.log("Drawing!")
  }
});

