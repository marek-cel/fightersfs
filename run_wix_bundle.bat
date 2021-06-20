call "%WIX%bin\candle.exe" -ext WixBalExtension wix_bundle.wxs
call "%WIX%bin\light.exe" -ext WixBalExtension -out fightersfs-0.1-win64.exe wix_bundle.wixobj
