cmd_/home/heng/os2/module2/modules.order := {   echo /home/heng/os2/module2/mydemo2.ko; :; } | awk '!x[$$0]++' - > /home/heng/os2/module2/modules.order
