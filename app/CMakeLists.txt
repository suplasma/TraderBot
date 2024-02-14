set(TARGET gen )

qt_add_executable(${TARGET} qcustomplot)

use_sys_control(${TARGET})
use_sys_common(${TARGET})
