SET PATH=C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\110\Tools\Binn\;C:\Program Files (x86)\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files\Microsoft SQL Server\120\DTS\Binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files (x86)\Windows Kits\10\Windows Performance Toolkit\;C:\Program Files\dotnet\;C:\Cadence\Orcad_9.2.3\tools\Capture;C:\Cadence\Orcad_9.2.3\tools\bin;C:\Cadence\Orcad_9.2.3\tools\jre\bin;C:\Cadence\Orcad_9.2.3\tools\fet\bin;C:\Cadence\Orcad_9.2.3\tools\specctra\bin;C:\Program Files\Git\cmd;C:\Program Files\nodejs\;C:\Program Files\MATLAB\R2025b\runtime\win64;C:\Program Files\MATLAB\R2025b\bin;C:\Users\Knnn\AppData\Local\Programs\Python\Python310\Scripts\;C:\Users\Knnn\AppData\Local\Programs\Python\Python310\;C:\Users\Knnn\AppData\Local\Programs\Python\Python312\Scripts\;C:\Users\Knnn\AppData\Local\Programs\Python\Python312\;C:\Users\Knnn\AppData\Local\Programs\Microsoft VS Code\bin;C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Git;C:\Users\Knnn\AppData\Local\Microsoft\WindowsApps;C:\Users\Knnn\AppData\Local\GitHubDesktop\bin;C:\Users\Knnn\AppData\Roaming\npm
SET CPU_TYPE=STM32F407VETx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=STM32CubeMX
SET CPU_CLOCK=0x017D7840
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\startup_stm32f407xx._ac"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\main.__i"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\stm32f4xx_it.__i"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\stm32f4xx_ll_gpio.__i"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\stm32f4xx_ll_rcc.__i"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\stm32f4xx_ll_utils.__i"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\stm32f4xx_ll_exti.__i"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\stm32cubemx\system_stm32f4xx.__i"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via ".\STM32CubeMX\STM32CubeMX.lnp"
"C:\Users\Knnn\AppData\Local\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" ".\STM32CubeMX\STM32CubeMX.axf" --i32combined --output ".\STM32CubeMX\STM32CubeMX.hex"
fromelf --bin --output "C:\Users\Knnn\Desktop\STM32_F407\test_F407\STM32CubeMX\Target_1\STM32CubeMX\Src\test_F407.bin" "C:\Users\Knnn\Desktop\STM32_F407\test_F407\STM32CubeMX\Target_1\STM32CubeMX\MDK-ARM\STM32CubeMX\STM32CubeMX.axf"
