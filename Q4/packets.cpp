#include "packets.hpp"

void serializeEdgesPacket(const edges_Packet &packet, std::vector<char> &buffer)
{
    int net_n = htonl(packet.n);
    int net_m = htonl(packet.m);

    buffer.resize(sizeof(int) * 2); // Reserve space for n and m
    std::memcpy(buffer.data(), &net_n, sizeof(int));
    std::memcpy(buffer.data() + sizeof(int), &net_m, sizeof(int));

    for (const auto &lst : packet.edges)
    {
        int list_size = htonl(lst.size());
        buffer.insert(buffer.end(), reinterpret_cast<char *>(&list_size), reinterpret_cast<char *>(&list_size) + sizeof(int));

        for (int value : lst)
        {
            int net_value = htonl(value);
            buffer.insert(buffer.end(), reinterpret_cast<char *>(&net_value), reinterpret_cast<char *>(&net_value) + sizeof(int));
        }
    }
}

void deserializeEdgesPacket(const char *buffer, edges_Packet &packet)
{
    const char *ptr = buffer;

    int net_n, net_m;
    memcpy(&net_n, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&net_m, ptr, sizeof(int));
    ptr += sizeof(int);

    packet.n = ntohl(net_n);
    packet.m = ntohl(net_m);

    packet.edges.resize(packet.m);

    for (int i = 0; i < packet.m; ++i)
    {
        int net_list_size;
        memcpy(&net_list_size, ptr, sizeof(int));
        ptr += sizeof(int);

        int list_size = ntohl(net_list_size);
        for (int j = 0; j < list_size; ++j)
        {
            int net_value;
            memcpy(&net_value, ptr, sizeof(int));
            ptr += sizeof(int);

            packet.edges[i].push_back(ntohl(net_value));
        }
    }
}

void serializeOpPacket(const op_Packet &packet, std::vector<char> &buffer)
{
    int op_length = packet.operation.size();
    int net_op_length = htonl(op_length);
    int net_v1 = htonl(packet.v1);
    int net_v2 = htonl(packet.v2);

    buffer.resize(sizeof(int) * 3 + op_length); // Reserve space for op_length, v1, v2, and operation

    memcpy(buffer.data(), &net_op_length, sizeof(int));
    memcpy(buffer.data() + sizeof(int), packet.operation.data(), op_length);
    memcpy(buffer.data() + sizeof(int) + op_length, &net_v1, sizeof(int));
    memcpy(buffer.data() + sizeof(int) + op_length + sizeof(int), &net_v2, sizeof(int));
}

void deserializeOpPacket(const char *buffer, op_Packet &packet)
{
    const char *ptr = buffer;

    int net_op_length;
    memcpy(&net_op_length, ptr, sizeof(int));
    ptr += sizeof(int);

    int op_length = ntohl(net_op_length);
    packet.operation.assign(ptr, op_length);
    ptr += op_length;

    int net_v1, net_v2;
    memcpy(&net_v1, ptr, sizeof(int));
    ptr += sizeof(int);
    memcpy(&net_v2, ptr, sizeof(int));

    packet.v1 = ntohl(net_v1);
    packet.v2 = ntohl(net_v2);
}
