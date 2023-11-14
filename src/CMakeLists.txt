# build group from raw data
add_executable(raw_to_group raw_to_group.cc transformer/RawGroupTrans.cc)
install(TARGETS raw_to_group DESTINATION ./) 

# build root from group file
add_executable(group_to_root group_to_root.cc transformer/GroupRootTrans.cc)
install(TARGETS group_to_root DESTINATION ./) 

# read raw data as text
add_executable(raw_reader raw_reader.cc)
install(TARGETS group_to_root DESTINATION ./) 