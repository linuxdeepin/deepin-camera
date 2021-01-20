rm -f ~/Pictures/相机/*.jpg
cp -r ~/Pictures/Wallpapers/*.jpg ~/Pictures/相机/
rm -rf ../build-ut
mkdir ../build-ut
cd ../build-ut
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j16
#mv asan.log* asan_${PROJECT_REALNAME}.log

workdir=$(cd ../$(dirname $0)/build-ut; pwd)
PROJECT_NAME=deepin-camera-test #可执行程序的文件名
PROJECT_REALNAME=deepin-camera  #项目名称
mkdir -p html
mkdir -p report
     echo " ===================CREAT LCOV REPROT==================== "
     lcov --directory ./tests/CMakeFiles/${PROJECT_REALNAME}.dir --zerocounters
     ./tests/${PROJECT_NAME} --gtest_output=xml:./report/report_deepin-camera.xml
     lcov --directory . --capture --output-file ./html/${PROJECT_REALNAME}_Coverage.info
     echo " =================== do filter begin ==================== "
     lcov --remove ./html/${PROJECT_REALNAME}_Coverage.info 'CMakeFiles/${PROJECT_NAME}.dir/deepin-camera-test_autogen/*/*' '${PROJECT_NAME}_autogen/*/*' 'googletest/*/*' '*/usr/include/*' '*/tests/*' '*/camera/src/qtsingleapplication/*' '*/camera/src/basepub/printoptionspage.cpp' '*/camera/src/dbus_adpator.cpp' '*/camera/src/settings_translation.cpp' '/usr/local/*' '*/config.h' -o ./html/${PROJECT_REALNAME}_Coverage_fileter.info
     echo " =================== do filter end ==================== "

     genhtml -o ./html ./html/${PROJECT_REALNAME}_Coverage_fileter.info
     echo " -- Coverage files have been output to ${CMAKE_BINARY_DIR}/html"
     mv ./html/index.html ./html/cov_${PROJECT_REALNAME}.html
     mv asan.log* asan_${PROJECT_REALNAME}.log
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
exit 0
