#pragma once
#include "pin_declaration.h"
#include "pin_mapping.h"
#include "input_augmentation.h"
#include "config.h"

enum Commands{
    Reset,
    TextCommand,
    ListConfigs,
    ListPins,
    ListMappings,
    ListAugments,
    ClearConfig,
    ClearPin,
    ClearMap,
    ClearAugments,
    //adds/edits item (depending on type)
    PinDeclaration,
    InputMapping,
    Augmentation,
};

void parse_serial();
void parse_line();

void clear_pin(u_int8_t index = 255);
void clear_mapping(u_int8_t index = 255);
void clear_augment(u_int8_t index = 255);

void print_pins(u_int8_t index = 255);
void print_pin(u_int8_t index);
void print_mappings(u_int8_t index = 255);
void print_mapping(u_int8_t index);
void print_augments(u_int8_t index = 255);
void print_augment(u_int8_t index);

//TODO - this should be calculated based off of the start code and message length bytes
//2 bytes for start code, 2 bytes for payload length, >=1 byte for payload, and 1 byte for checksum
#define MINIMUM_MESSAGE_LENGTH 3
void parse_serial() {
    if (Serial.available() > MINIMUM_MESSAGE_LENGTH) {
        //Serial.println("Parsing serial");
        parse_line();
    }
}

void parse_line() {
    //first two byes are the start code
    char first_byte = Serial.read();
    char second_byte = Serial.read();

    if (first_byte != 0xFF or second_byte != 0x00) {
        //This indicates either some sort of failure, or garbage data being sent.
        //flush the buffer
        while(Serial.available() > 0) Serial.read();

        //idk print an error here or something
        Serial.println("First bytes were not start code. Aborting.");
        return;
    }
     
    u_int16_t payload_size = (static_cast<u_int16_t>((u_int8_t) Serial.read()) << 8) | static_cast<u_int16_t>((u_int8_t) Serial.read());
    u_int8_t payload_buffer[payload_size];
    u_int16_t bytes_read = Serial.readBytes(payload_buffer, payload_size);

    //TODO add better checks to the lenght of the payload buffer

    //this means the readBytes function timed out before all data was read
    if ((payload_size) != bytes_read) {
        Serial.println("Serial.readBytes() timed out before reading all data. expected bytes=" + (String) (payload_size) + " bytes read=" + (String) bytes_read);
        //flush the buffer
        while(Serial.available() > 0) Serial.read();
        return;
    }

    u_int8_t checksum_value = Serial.read();
    
    //flush the buffer
    if (Serial.available() > 0) {
        Serial.println("More serial data is available than expected. Aborting.");
        return;
    }

    int sum = 0;
    for (u_int8_t byte : payload_buffer) {
        sum += byte;
    }
    sum = sum & 0xFF;

    if (checksum_value != sum) {
        Serial.println("Checksum did not match: Expected checksum=" + (String) checksum_value + " calculated checksum=" + (String) sum);
        return;
    }

    //keeps track of how far into the payload we are.
    int message_index = 0;
    
    u_int8_t command = payload_buffer[message_index];
    message_index += 1; //increase the message index for that first byte that we consumed
    switch (command) {
        case Reset: {
            Serial.println("Reset");
            Serial.flush();
            NVIC_SystemReset();
        }
        case ListConfigs: {
            
            Serial.println("Pins:");
            print_pins();

            Serial.println("Mappings:");
            print_mappings();

            Serial.println("Augments:");
            print_augments();

            break;
        }
        case ListPins: {
            u_int8_t index = payload_buffer[message_index];
            message_index += 1; //increase the message index for the 1 byte that we just consumed
            Serial.println("Printing pin at index=" + (String) index);
            print_pins(index);
            break;
        }
        case ListMappings: {
            int index = payload_buffer[message_index];
            message_index += 1; //increase the message index for the 1 byte that we just consumed
            Serial.println("Printing mapping at index=" + (String) index);
            print_mappings(index);
            break;
        }
        case ListAugments: {
            int index = payload_buffer[message_index];
            message_index += 1; //increase the message index for the 1 byte that we just consumed
            Serial.println("Printing augment at index=" + (String) index);
            print_augments(index);
            break;
        }
        case ClearConfig: {
            clear_pin();
            clear_mapping();
            clear_augment();
            Serial.println("Configs cleared");
            break;
        }
        case ClearPin: {
            int index = payload_buffer[message_index];
            message_index += 1; //increase the message index for the 1 byte that we just consumed
            //TODO: Add index to clear specific item
            clear_pin(index);
            Serial.println("Pins cleared");
            break;
        }
        case ClearMap: {
            int index = payload_buffer[message_index];
            message_index += 1; //increase the message index for the 1 byte that we just consumed
            //TODO: Add index to clear specific item
            clear_mapping(index);
            //not sure if I should actually deallocate memory, so i'll keep it simple for now.
            //pin_bindings.shrink_to_fit();
            Serial.println("Mapping cleared");
            break;
        }
        case ClearAugments: {
            int index = payload_buffer[message_index];
            message_index += 1; //increase the message index for the 1 byte that we just consumed
            //TODO: Add index to clear specific item
            clear_augment(index);
            //not sure if I should actually deallocate memory, so i'll keep it simple for now.
            //augmentations.shrink_to_fit();
            Serial.println("Augments cleared");
            break;
        }


        case PinDeclaration: {
            //syntax: int pin_number, bool analog
            PinName pin = (PinName) payload_buffer[message_index + 0];
            bool analog = payload_buffer[message_index + 1];
            message_index += 2; //increase the message index for the 2 bytes that we just consumed
            pin_map[pin] = Pin(pin, analog);
            Serial.println("Added new pin: pin number=" + (String) pin + " analog=" + (String) analog);
            break;
        }
        case InputMapping: {
            PinName pin = (PinName) payload_buffer[message_index + 0];
            int input_device = payload_buffer[message_index + 1];
            int input_id = payload_buffer[message_index + 2];
            bool invert = payload_buffer[message_index + 3];
            message_index += 4; //increase the message index for the 5 bytes that we just consumed
            
            //for some reason, when added at runtime, this new pinmapping doesn't work.
            pin_bindings.push_back(PinMapping(pin, input_device, input_id, invert));
            Serial.println("Added new mapping: pin number=" + (String) pin + " input device=" + (String) input_device + " input id=" + (String) input_id + " inverted=" + (String) invert);
            break;
        }
        case Augmentation: {

            break;
        }
    }
}








void clear_pin(u_int8_t index) {
    if (index == 255) {
        pin_map.clear();
        return;
    }

    auto it = pin_map.find(index);

    if (it == pin_map.end()) {
        Serial.println("Pin not found: pin=" + (String) index);
        return;
    }

    pin_map.erase(index);
}
void clear_mapping(u_int8_t index) {
    if (index == 255) {
        pin_bindings.clear();
        return;
    }

    if (index >= 0 and index < pin_bindings.size()) {
        pin_bindings.erase(pin_bindings.begin() + index);
        //pin_bindings.shrink_to_fit();
    }
}
void clear_augment(u_int8_t index) {
    if (index == 255) {
        augmentations.clear();
        return;
    }

    if (index >= 0 and index < augmentations.size()) {
        augmentations.erase(augmentations.begin() + index);
        augmentations.shrink_to_fit();
    }
}



void print_pins(u_int8_t index) {
    if (index != 255) {
        print_pin(index);
        return;
    }

    for (const auto& [pin_index, pin] : pin_map) {
        print_pin((u_int8_t) pin_index);
    }
}
void print_pin(u_int8_t index) {
    auto it = pin_map.find(index);
    if (it == pin_map.end()) {
        Serial.println("Pin not found: pin=" + (String) index);
        return;
    }

    Pin& pin = it->second;

    Serial.println((String) index + 
    ": pin number=" + (String) pin.pin_name + 
    " analog=" + (String) pin.analog 
    );
}

void print_mappings(u_int8_t index) {
    if (index != 255) {
        print_mapping(index);
        return;
    }

    for (size_t mapping_index = 0; mapping_index < pin_bindings.size(); ++mapping_index) {
        print_mapping((u_int8_t) mapping_index);
    }
}
void print_mapping(u_int8_t index) {
    if (index > pin_bindings.size()) {
        Serial.println("mapping index out of range: index=" + (String) index);
        return;
    }
    PinMapping& mapping = pin_bindings[index];
    //TODO: figure out why sometimes pins print pin_name as NC (really large number)
    Serial.println((String) index + 
    ": pin number=" + (String) mapping.pin_name + 
    " input device=" + (String) mapping.input_type +
    " input id=" + (String) mapping.input_id +
    " inverted=" + (String) mapping.invert +
    " max report value=" + (String) mapping.max_report_value +
    " activation value=" + (String) mapping.activation_value +
    " scale=" + (String) mapping.scale +
    " deadzone percent=" + (String) mapping.deadzone_percent +
    " change amount before update=" + (String) mapping.change_amount_before_update +
    " quick release=" + (String) mapping.quick_release +
    " counter strafe help time ms=" + (String) mapping.counter_strafe_help_time_ms
    );
}

void print_augments(u_int8_t index) {
    if (index != 255) {
        print_augment(index);
        return;
    }

    for (size_t augment_index = 0; augment_index < augmentations.size(); ++augment_index) {
        print_augment((u_int8_t) augment_index);
    }
}
void print_augment(u_int8_t index) {
    if (index > augmentations.size()) {
        Serial.println("Pin index out of range: index=" + (String) index);
        return;
    }
    InputAugmentation& augment = augmentations[index];
    //TODO: figure out why sometimes pins print pin_name as NC (really large number)
    Serial.println((String) index + 
    ": pin number=" + (String) augment.pin +
    " secondary pin number=" + (String) augment.secondary_pin +
    " type=" + (String) augment.type +
    " rotation=" + (String) augment.control_rotation
    );
}