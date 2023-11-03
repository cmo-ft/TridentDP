# build group from raw data
add_executable(raw_to_group raw_to_group.cc transformer/RawGroupTrans.cc)

# build root from group file
add_executable(group_to_root group_to_root.cc transformer/GroupRootTrans.cc)

# read raw data as text
add_executable(raw_reader raw_reader.cc)