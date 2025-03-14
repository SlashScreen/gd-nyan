#pragma once

#include <stdlib.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include "nyan/nyan/nyan.h"
using namespace godot;

std::string godot_string_to_std_string(const String &string)
{
    return std::string(string.utf8().get_data());
}

String std_string_to_godot_string(const std::string &string)
{
    return String::utf8(string.c_str(), string.length());
}

class NyanDatabase : public RefCounted
{
    GDCLASS(NyanDatabase, RefCounted);

protected:
    static void _bind_methods();

public:
    NyanDatabase();
    NyanDatabase(const std::shared_ptr<nyan::Database> &database)
    {
        this->database = database;
    };
    ~NyanDatabase();

    void load(const String &filename)
    {
        this->database->load(godot_string_to_std_string(filename), filefetcher);
    }

private:
    std::shared_ptr<nyan::Database> database;

    static std::shared_ptr<nyan::File> filefetcher(const std::string &filename)
    {
        ProjectSettings *project_settings = ProjectSettings::get_singleton();
        String file_path = project_settings->globalize_path(std_string_to_godot_string(filename));
        return std::make_shared<nyan::File>(godot_string_to_std_string(file_path));
    }
};
