create_include_link(${BAL_DIR}/Memory/WTF/BCFastMallocWTF.h BAL/wtf/FastMalloc.h)
create_include_link(${BAL_DIR}/Memory/WTF/BCTCSystemAllocWTF.h BAL/wtf/TCSystemAlloc.h)
create_include_link(${BAL_DIR}/Memory/WTF/BCMallocZoneSupportWTF.h BAL/wtf/MallocZoneSupport.h)

if(ENABLE_DEBUG)
    aux_source_directory(${BAL_DIR}/Memory/WTF WTF_SRC)
endif(ENABLE_DEBUG)
