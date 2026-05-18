script_folder="/home/burger_r/devel/conan/service_provider_canopen_protocol/conan"
echo "echo Restoring environment" > "$script_folder/deactivate_conanrunenv-relwithdebinfo-x86_64.sh"
for v in LN_MESSAGE_DEFINITION_DIRS PATH LD_LIBRARY_PATH DYLD_LIBRARY_PATH
do
   is_defined="true"
   value=$(printenv $v) || is_defined="" || true
   if [ -n "$value" ] || [ -n "$is_defined" ]
   then
       echo export "$v='$value'" >> "$script_folder/deactivate_conanrunenv-relwithdebinfo-x86_64.sh"
   else
       echo unset $v >> "$script_folder/deactivate_conanrunenv-relwithdebinfo-x86_64.sh"
   fi
done

export LN_MESSAGE_DEFINITION_DIRS="$LN_MESSAGE_DEFINITION_DIRS:/volume/conan_cache/burger_r/.conan2/p/servi75e01df3d11c1/p/share/ln/message_definitions"
export PATH="/volume/conan_cache/burger_r/.conan2/p/robotc5782ce2b811e/p/bin:$PATH:/volume/conan_cache/burger_r/.conan2/p/robotc5782ce2b811e/p/bin:/volume/conan_cache/burger_r/.conan2/p/robotc5782ce2b811e/p/bin"
export LD_LIBRARY_PATH="/volume/conan_cache/burger_r/.conan2/p/robotc5782ce2b811e/p/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="/volume/conan_cache/burger_r/.conan2/p/robotc5782ce2b811e/p/lib:$DYLD_LIBRARY_PATH"