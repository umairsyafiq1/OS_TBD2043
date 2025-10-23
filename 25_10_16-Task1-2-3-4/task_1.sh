echo -n "Enter the Number: " 
read n

r=$((n % 2))

if [ $r -eq 0 ]
then
    echo "$n is an Even number"
else
    echo "$n is an Odd number"
fi
