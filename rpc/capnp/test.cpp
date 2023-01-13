#include <iostream>
#include <capnp/serialize.h>
#include "test.capnp.h"

int main()
{
    // Encode message
    ::capnp::MallocMessageBuilder message_builder;
    Testmessage::Builder message = message_builder.initRoot<Testmessage>();

    message.setString( "string" );
    message.setFloat( 3.14 );
    message.setInt( 1337 );

    auto encoded_array = capnp::messageToFlatArray(message_builder);
    //auto encoded_array_ptr = encoded_array.asChars();
    //auto encoded_char_array = encoded_array_ptr.begin();
    //auto size = encoded_array_ptr.size();

    // Send message
    // Receive message

    // Decode message
    auto received_array = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(encoded_array.asChars().begin()), encoded_array.asChars().size()/sizeof(capnp::word));
    ::capnp::FlatArrayMessageReader message_receiver_builder(received_array);
    auto message_receiver = message_receiver_builder.getRoot<Testmessage>();
    auto s_r = message_receiver.getString().cStr();
    auto f_r = message_receiver.getFloat();
    auto i_r = message_receiver.getInt();

    std::cout << "received: " << s_r << ", " << f_r << ", " << i_r << std::endl;
}