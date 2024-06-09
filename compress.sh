rm ./distro/GDedit-linux.tar.gz
rm ./distro/GDedit-win.zip

echo "Compressing Windows..."
zip -r -q ./distro/GDedit-win.zip ./distro/win
echo "Compressing Linux..."
tar czf ./distro/GDedit-linux.tar.gz ./distro/linux 
echo "Successfully compressed Windows and Linux versions."