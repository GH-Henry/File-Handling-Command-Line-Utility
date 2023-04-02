#!/bin/bash

echo "Unknown Operation test"

#Run the -i command without anything after. It should display unknown option error message
output="$(./extfat -i test.image)"
errorMessage="Unknown option
Try './extfat -h' for more information"

# Compare the output with the expected error message
if [[ $output == $errorMessage ]]; then
    result=0
else
    result=1
fi

exit $result
