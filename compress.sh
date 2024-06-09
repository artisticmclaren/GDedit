rm ./distro/GDedit-linux.tar.gz
rm ./distro/GDedit-win.zip
rm ./distro/linux/saves/*
rm ./distro/win/saves/*

echo "Compressing Windows..."
zip -r -q ./distro/GDedit-win.zip ./distro/win
echo "Compressing Linux..."
tar czf ./distro/GDedit-linux.tar.gz ./distro/linux 
echo "Successfully compressed Windows and Linux versions."