# Mikan-OS 写経

- [写経元リポジトリ](https://github.com/uchan-nos/mikanos)
- みかん本に写経を推されたので


## 環境構築

```bash
# Initialize
sudo apt update && sudo apt install git -y && cd ~
git clone https://github.com/uchan-nos/mikanos-build.git osbook
cd osbook && git checkout 8d4882122ec548ef680b6b5a2ae841a0fd4d07a1  # Ubuntu 18.04 or 20.04
sudo apt install ansible -y && cd ~/osbook/devenv
# Run ansible
ansible-playbook -K -i ansible_inventory ansible_provision.yml
# Check the environment
iasl -v
ls ~/edk2
# Link MikanLoaderPkg to edk2
ln -s /path/to/this/repository/MikanLoaderPkg ~/edk2/MikanLoaderPkg
# Check whether linking is successful
ls ~/edk2/MikanLoaderPkg
# Build and run (Launch QEMU)
/path/to/this/repository/run_kernel
```
