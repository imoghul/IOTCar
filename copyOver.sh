cp -R "$1" "$2"
rm "$2/$1 simple.dep"
mv "$2/$1 simple.ewd" "$2/$2 simple.ewd"
mv "$2/$1 simple.eww" "$2/$2 simple.eww"
mv "$2/$1 simple.ewt" "$2/$2 simple.ewt"
mv "$2/$1 simple.ewp" "$2/$2 simple.ewp"

sed -i "s/$1/$2/g" "$2/$2 simple.ewd" "$2/$2 simple.eww"
