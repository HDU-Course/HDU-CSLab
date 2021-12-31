cmd_/home/heng/os2/module1/Module.symvers := sed 's/\.ko$$/\.o/' /home/heng/os2/module1/modules.order | scripts/mod/modpost -m -a  -o /home/heng/os2/module1/Module.symvers -e -i Module.symvers   -T -
