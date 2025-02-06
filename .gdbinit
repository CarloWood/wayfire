set environment WLR_BACKENDS=headless
set environment WLR_RENDER_DRM_DEVICE=/dev/dri/renderD128
#set environment LD_PRELOAD=/usr/local/install/14.2.1-20250128/lib/libmemleak.so

python
import os
gdb.execute('directory $cdir:$cwd:' + os.environ['BUILDDIR'])
end
