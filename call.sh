
echo Start failing clone
./failing_clone --rose:openmp:ast_only input_failing_clone.c

echo Start succeeding clone
./succeeding_clone --rose:openmp:ast_only input_succeeding_clone.c
