struct ExosDatasetInfo
    name::Cstring
    adr::Ptr{Cvoid}
    size::Csize_t
    offset::Clong
    array_items::NTuple{EXOS_ARRAY_DEPTH, Cuint}
end

function JExosDatasetInfo(name::String, adr::Ref{T}, size::UInt64, offset::Int64,
                          array_items::NTuple{EXOS_ARRAY_DEPTH, UInt32}) where T
    c_name = Base.unsafe_convert(Cstring,  name)
    c_adr = pointer_from_objref(adr)
    ExosDatasetInfo(c_name, c_adr, size, offset, array_items)
end

struct ExosDatamodelSyncInfo
    in_sync::Cuchar
    _reserved_bool::NTuple{8, Cuchar}
    missed_dmr_cycles::Cuint
    missed_ar_cycles::Cuint
    proccess_mode::EXOS_DATAMODEL_PROCESS_MODE
    _reserved_uint32::NTuple{7, Cuint}
end

function JExosDatamodelSyncInfo(in_sync::Cuchar,
                                _reserved_bool::NTuple{8, Cuchar},
                                missed_dmr_cycles::Cuint,
                                missed_ar_cycles::Cuint,
                                proccess_mode::EXOS_DATAMODEL_PROCESS_MODE,
                                _reserved_uint32::NTuple{7, Cuint})
    ExosDatamodelSyncInfo(in_sync, _reserved_bool, missed_dmr_cycles,
                          missed_ar_cycles, proccess_mode, _reserved_uint32)
end

struct ExosLogPrivate
    _magic::Cuint
    _log::Ptr{Cvoid}
    _reserved::NTuple{4, Ptr{Cvoid}}
end

function JExosLogPrivate(_magic::UInt32, _log::Ref{R},
                         _reserved::NTuple{4, Ref{T}}) where {R, T}
    c__reserved_tuple = ()
    for i in _reserved
        c__reserved_tuple = tuple(c__reserved_tuple..., pointer_from_objref(i))
    end
    c__log = pointer_from_objref(_log)
    ExosLogPrivate(_magic, c__log, c__reserved_tuple)
end

struct ExosDatamodelPrivate
    _magic::Cuint
    _artefact::Ptr{Cvoid}
    _reserved::NTuple{8, Ptr{Cvoid}}
end

function JExosDatamodelPrivate(_magic::UInt32, _artefact::Ref{R},
                               _reserved::NTuple{8, Ref{T}}) where {R, T}
    c__reserved_tuple = ()
    for i in _reserved
        c__reserved_tuple = tuple(c__reserved_tuple..., pointer_from_objref(i))
    end
    c__artefact = pointer_from_objref(_artefact)
    ExosDatamodelPrivate(_magic, c__artefact, c__reserved_tuple)
end

mutable struct ExosLogHandle
    name::Cstring
    ready::Cuchar
    excluded::Cuchar
    console::Cuchar
    config_change_cb::Ptr{Cvoid}
    config_change_user_context::Ptr{Cvoid}
    _reserved_bool::NTuple{4, Cuchar}
    _reserved_uint32::NTuple{4, Cuint}
    _reserved::NTuple{4, Ptr{Cvoid}}
    _private::ExosLogPrivate
end
function JExosLogHandle(name::String,
                        ready::UInt8,
                        excluded::UInt8,
                        console::UInt8,
                        config_change_cb::Ptr{Cvoid},
                        config_change_user_context::Ref{R},
                        _reserved_bool::NTuple{4, UInt8},
                        _reserved_int32::NTuple{4, UInt32},
                        _reserved::NTuple{4, Ref{T}},
                        _private::ExosLogPrivate) where {R, T}
    c__reserved_tuple = ()
    for i in _reserved
        c__reserved_tuple = tuple(c__reserved_tuple..., pointer_from_objref(i))
    end

    c__reserved_tuple
    c_name =  Base.unsafe_convert(Cstring,  name)
    c_config_change_user_context = pointer_from_objref(config_change_user_context)
    ExosLogHandle(c_name, ready, excluded, console,
                    config_change_cb, c_config_change_user_context,
                    _reserved_bool, _reserved_int32,
                    c__reserved_tuple, _private)
end

mutable struct ExosDatamodelHandle
    name::Cstring
    connection_state::EXOS_CONNECTION_STATE
    error::EXOS_ERROR_CODE
    user_context::Ptr{Cvoid}
    user_tag::Clong
    user_alias::Cstring
    exos_datamodel_event_cb::Ptr{Cvoid}
    sync_info::ExosDatamodelSyncInfo
    _reserved_bool::NTuple{8, Cuchar}
    _reserved_uint32::NTuple{8, Cuint}
    _reserved_void::NTuple{8, Ptr{Cvoid}}
    _private::ExosDatamodelPrivate
end

function JExosDatamodelHandle(name::String,
                              connection_state::EXOS_CONNECTION_STATE,
                              error::EXOS_ERROR_CODE,
                              user_context::Ref{R},
                              user_tag::Int,
                              user_alias::String,
                              exos_datamodel_event_cb::Ptr{Cvoid},
                              sync_info::ExosDatamodelSyncInfo,
                              _reserved_bool::NTuple{8, UInt8},
                              _reserved_uint32::NTuple{8, UInt32},
                              _reserved_void::NTuple{8, Ref{T}},
                              private::ExosDatamodelPrivate) where {R, T}
    c__reserved_void_tuple = ()
    for i in _reserved_void
        c__reserved_void_tuple = tuple(c__reserved_void_tuple..., pointer_from_objref(i))
    end

    c_name =  Base.unsafe_convert(Cstring,  name)
    c_user_alias =  Base.unsafe_convert(Cstring, user_alias)
    c_user_context = pointer_from_objref(user_context)
    c__reserved_void_tuple

    ExosDatamodelHandle(c_name, connection_state, error, c_user_context,
                        user_tag, c_user_alias, exos_datamodel_event_cb,
                        sync_info, _reserved_bool, _reserved_uint32,
                        c__reserved_void_tuple, private)
end

struct ExosBufferInfo
    size::Cuint
    free::Cuint
    used::Cuint
end

function JExosBufferInfo(size::UInt32, free::UInt32, used::UInt32)
    ExosBufferInfo(size, free, used)
end

struct ExosDatasetPrivate
    _magic::Cuint
    _value::Ptr{Cvoid}
    _reserved::NTuple{8, Ptr{Cvoid}}
end

function JExosDatasetPrivate(_magic::UInt32, _value::Ref{R},
    _reserved::NTuple{8, Ref{T}})  where {R, T}
    c__reserved_tuple = ()
    for i in _reserved
        c__reserved_tuple = tuple(c__reserved_tuple..., pointer_from_objref(i))
    end
    c__value = pointer_from_objref(_value)

    ExosDatasetPrivate(_magic, c__value, c__reserved_tuple)
end

mutable struct ExosDatasetHandle
    name::Cstring
    type::EXOS_DATASET_TYPE
    datamodel::ExosDatamodelHandle # NOTE! This type is exos_datamodel_handle_t*
                                   # in C. The reason this is fine being a non-Ptr
                                   # value in Julia is because a mutable type is
                                   # already stored as type* in the Julia struct
                                   # layout, meaning this is equivalent to C.
    data::Ptr{Cvoid}
    size::Csize_t
    error::EXOS_ERROR_CODE
    connection_state::EXOS_CONNECTION_STATE
    send_buffer::ExosBufferInfo
    nettime::Cint
    user_tag::Cint
    user_context::Ptr{Cvoid}
    dataset_event_callback::Ptr{Cvoid}
    _reserved_bool::NTuple{8,Cuchar}
    _reserved_uint32::NTuple{8,Cuint}
    _reserved_void::NTuple{8, Ptr{Cvoid}}
    _private::ExosDatasetPrivate
end

function JExosDatasetHandle(name::String,
                            type::EXOS_DATASET_TYPE,
                            datamodel::ExosDatamodelHandle,
                            data::Ref{R},
                            size::UInt64,
                            error::EXOS_ERROR_CODE,
                            connection_state::EXOS_CONNECTION_STATE,
                            send_buffer::ExosBufferInfo,
                            nettime::Int32,
                            user_tag::Int32,
                            user_context::Ref{W},
                            dataset_event_callback::Ptr{Cvoid},
                            _reserved_bool::NTuple{8, UInt8},
                            _reserved_uint32::NTuple{8, UInt32},
                            _reserved_void::NTuple{8, Ref{T}},
                            _private::ExosDatasetPrivate) where {R, W, T}
    c__reserved_void_tuple = ()
    for i in _reserved_void
        c__reserved_void_tuple = tuple(c__reserved_void_tuple..., pointer_from_objref(i))
    end

    c_name = Base.unsafe_convert(Cstring, name)::Cstring
    c_data = pointer_from_objref(data)
    c_user_context = pointer_from_objref(user_context)
    c__reserved_void_tuple

    ExosDatasetHandle(c_name, type, datamodel, c_data, size, error,
                      connection_state, send_buffer, nettime, user_tag,
                      c_user_context, dataset_event_callback, _reserved_bool,
                      _reserved_uint32, c__reserved_void_tuple, _private)
end

struct ExosLogConfigType
    user::Cuchar
    system::Cuchar
    verbose::Cuchar
    _reserved_bool::NTuple{8, Cuchar}
end

function JExosLogConfigType(user::UInt8, system::UInt8,
                            verbose::UInt8, _reserved_bool::NTuple{8, UInt8})
    ExosLogConfigType(user, system, verbose, _reserved_bool)
end

struct ExosLogConfig
    level::EXOS_LOG_LEVEL
    type::ExosLogConfigType
    _reserved_int32::NTuple{4,Cint}
    _reserved_bool::NTuple{4, Cuchar}
    exclude::NTuple{EXOS_LOG_EXCLUDE_LIST_LEN,
             NTuple{EXOS_LOG_MAX_NAME_LENGTH + 1, Cchar}}
end

function JExosLogConfig(level::EXOS_LOG_LEVEL, type::ExosLogConfigType,
                        _reserved_int32::NTuple{4, Int32},
                        _reserved_bool::NTuple{4, UInt8},
                        exclude::NTuple{EXOS_LOG_EXCLUDE_LIST_LEN,
                                 NTuple{EXOS_LOG_MAX_NAME_LENGTH + 1, Int8}})
    ExosLogConfig(level, type, _reserved_int32, _reserved_bool, exclude)
end
