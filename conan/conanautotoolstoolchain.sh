script_folder="/home/burger_r/devel/conan/service_provider_canopen_protocol/conan"
echo "echo Restoring environment" > "$script_folder/deactivate_conanautotoolstoolchain.sh"
for v in CPPFLAGS CXXFLAGS CFLAGS LDFLAGS PKG_CONFIG_PATH
do
   is_defined="true"
   value=$(printenv $v) || is_defined="" || true
   if [ -n "$value" ] || [ -n "$is_defined" ]
   then
       echo export "$v='$value'" >> "$script_folder/deactivate_conanautotoolstoolchain.sh"
   else
       echo unset $v >> "$script_folder/deactivate_conanautotoolstoolchain.sh"
   fi
done

export CPPFLAGS="$CPPFLAGS -DNDEBUG"
export CXXFLAGS="$CXXFLAGS -m64 -O2 -g -pthread -march=x86-64 -mfpmath=sse -msahf -mfxsr -mcx16 -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -mpopcnt"
export CFLAGS="$CFLAGS -m64 -O2 -g -pthread -march=x86-64 -mfpmath=sse -msahf -mfxsr -mcx16 -mmmx -msse -msse2 -msse3 -mssse3 -msse4 -mpopcnt"
export LDFLAGS="$LDFLAGS -m64"
export PKG_CONFIG_PATH="$script_folder/../conan:$PKG_CONFIG_PATH"