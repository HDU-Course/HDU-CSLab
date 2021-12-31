cmd_/home/heng/os2/module1/modules.order := {   echo /home/heng/os2/module1/mydemo.ko; :; } | awk '!x[$$0]++' - > /home/heng/os2/module1/modules.order
