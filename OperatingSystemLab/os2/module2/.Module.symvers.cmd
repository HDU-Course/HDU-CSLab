cmd_/home/heng/os2/module2/Module.symvers := sed 's/\.ko$$/\.o/' /home/heng/os2/module2/modules.order | scripts/mod/modpost -m -a  -o /home/heng/os2/module2/Module.symvers -e -i Module.symvers   -T -
