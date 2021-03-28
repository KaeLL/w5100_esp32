
idf_component_register(
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "priv_includes"
    SRCS "w5100.c" "w5100_socket.c" "w5100_ll.c" "w5100_internal.c" "eth_mac.c" "eth_main.c" "eth_phy.c"
)