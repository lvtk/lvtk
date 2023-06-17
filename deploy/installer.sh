#!/bin/bash
# Generate the installer.

rm -rf *.gz *.app *.dmg

@BINARYCREATOR@ -v -c 'config/config.xml' --ac 9 \
    -i 'org.lvtk.sdk,org.lvtk.lv2,org.lvtk.plugins' \
    -p 'packages' @INSTALLERBASE@

sed -i '' 's/com.yourcompany.installerbase/org.lvtk.installer/' \
    "@INSTALLERBASE@.app/Contents/Info.plist"

@MACDEPLOYQT@ @INSTALLERBASE@.app \
    -no-plugins -dmg -always-overwrite -verbose=3 \
    @MACDEPLOYQT_ARGS@

@NOTARYSCRIPT@

rm -rf archives && mkdir archives
cd packages

@ARCHIVEGEN@ -f tar.gz -c 9  ../archives/org.lvtk.archives.tar.gz \
    org.lvtk.lv2 org.lvtk.sdk org.lvtk.plugins

cd ..
cd archives
md5 *.* > md5.sum
