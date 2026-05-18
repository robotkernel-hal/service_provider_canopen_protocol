script_folder="/home/burger_r/devel/conan/service_provider_canopen_protocol/conan"
echo "echo Restoring environment" > "$script_folder/deactivate_conanbuildenv-relwithdebinfo-x86_64.sh"
for v in NM RANLIB AR CPP CXX CC PATH PYTHONPATH LN_MESSAGE_DEFINITION_DIRS
do
   is_defined="true"
   value=$(printenv $v) || is_defined="" || true
   if [ -n "$value" ] || [ -n "$is_defined" ]
   then
       echo export "$v='$value'" >> "$script_folder/deactivate_conanbuildenv-relwithdebinfo-x86_64.sh"
   else
       echo unset $v >> "$script_folder/deactivate_conanbuildenv-relwithdebinfo-x86_64.sh"
   fi
done

export NM="gcc-nm-13"
export RANLIB="gcc-ranlib-13"
export AR="gcc-ar-13"
export CPP="cpp-13"
export CXX="g++-13"
export CC="gcc-13"
export PATH="/volume/conan_cache/burger_r/.conan2/p/robot31e82f0ba1c24/p/src:$PATH:/volume/conan_cache/burger_r/.conan2/p/robot31e82f0ba1c24/p/src"
export PYTHONPATH="$PYTHONPATH:/volume/conan_cache/burger_r/.conan2/p/robot31e82f0ba1c24/p/src"
export LN_MESSAGE_DEFINITION_DIRS="$LN_MESSAGE_DEFINITION_DIRS:/volume/conan_cache/burger_r/.conan2/p/servi75e01df3d11c1/p/share/ln/message_definitions"