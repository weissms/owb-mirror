##################################################
# Memory support part.                           #
##################################################

create_include_link(${BAL_DIR}/Memory/WebCore/WK/BCArenaWK.h BAL/Arena.h)
create_include_link(${BAL_DIR}/Memory/WebCore/WK/BCAutodrainedPoolWK.h BAL/AutodrainedPool.h)

aux_source_directory(${BAL_DIR}/Memory/WebCore/WK WEBCORE_SRC)
