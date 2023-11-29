enum_to_integer(enum::Enum) = UInt32(enum)

function exos_datamodel_calc_dataset_info(info::Vector{ExosDatasetInfo})
    info_size = sizeof(info)
    @ccall libexos_api.exos_datamodel_calc_dataset_info(info::Ref{ExosDatasetInfo},
                                                        info_size::Csize_t)::Cvoid
end

function exos_datamodel_init(datamodel::ExosDatamodelHandle,
                             datamodel_instance_name::String,
                             user_alias::String)
    GC.@preserve begin
        c_datamodel_instance_name = Base.unsafe_convert(Cstring,
                                                        datamodel_instance_name)
        c_user_alias = Base.unsafe_convert(Cstring, user_alias)
    end
    @ccall libexos_api.exos_datamodel_init(datamodel::Ref{ExosDatamodelHandle},
                                           c_datamodel_instance_name::Cstring,
                                           c_user_alias::Cstring)::EXOS_ERROR_CODE
end

function exos_dataset_init(dataset::ExosDatasetHandle,
                           datamodel::ExosDatamodelHandle,
                           browse_name::String,
                           data::Ref{T},
                           size::UInt64) where T
    GC.@preserve begin
        c_browse_name = Base.unsafe_convert(Cstring, browse_name)
        c_data = pointer_from_objref(data)
    end
    @ccall libexos_api.exos_dataset_init(dataset::Ref{ExosDatasetHandle},
                                         datamodel::Ref{ExosDatamodelHandle},
                                         c_browse_name::Cstring,
                                         c_data::Ptr{Cvoid},
                                         size::Csize_t)::EXOS_ERROR_CODE
end

function exos_datamodel_connect(datamodel::ExosDatamodelHandle,
                                config::String,
                                info::Vector{ExosDatasetInfo},
                                datamodel_event_callback::Ptr{Cvoid})
    info_size = sizeof(info)
    GC.@preserve c_config = Base.unsafe_convert(Cstring, config)
    @ccall libexos_api.exos_datamodel_connect(datamodel::Ref{ExosDatamodelHandle},
                                              c_config::Cstring,
                                              info::Ref{ExosDatasetInfo},
                                              info_size::Csize_t,
                                              datamodel_event_callback::Ptr{Cvoid}
                                              )::EXOS_ERROR_CODE
end

function exos_dataset_connect(dataset::ExosDatasetHandle,
                              type::Union{EXOS_DATASET_TYPE, UInt32},
                              dataset_event_callback::Ptr{Cvoid})
    if type isa EXOS_DATASET_TYPE
        type = enum_to_integer(type)
    end
    @ccall libexos_api.exos_dataset_connect(dataset::Ref{ExosDatasetHandle},
                                            type::Cuint,
                                            dataset_event_callback::Ptr{Cvoid}
                                            )::EXOS_ERROR_CODE
end

function exos_datamodel_process(datamodel_handle::ExosDatamodelHandle)
    @ccall libexos_api.exos_datamodel_process(
           datamodel_handle::Ref{ExosDatamodelHandle})::EXOS_ERROR_CODE
end

function exos_dataset_publish(dataset_handle::ExosDatasetHandle)
    @ccall libexos_api.exos_dataset_publish(
           dataset_handle::Ref{ExosDatasetHandle})::EXOS_ERROR_CODE
end

function exos_log_init(log_handle::ExosLogHandle, name::String)
    GC.@preserve c_name = Base.unsafe_convert(Cstring, name)
    @ccall libexos_api.exos_log_init(log_handle::Ref{ExosLogHandle},
                                     c_name::Cstring)::EXOS_ERROR_CODE
end

function exos_log_info(log::ExosLogHandle, type::EXOS_LOG_TYPE, format::String)
    GC.@preserve c_format = Base.unsafe_convert(Cstring, format)
    @ccall libexos_api.exos_log_info(log::Ref{ExosLogHandle},
                                     type::EXOS_LOG_TYPE,
                                     c_format::Cstring)::EXOS_ERROR_CODE
end

function exos_log_process(log::ExosLogHandle)
    @ccall libexos_api.exos_log_process(log::Ref{ExosLogHandle})::EXOS_ERROR_CODE
end

function exos_datamodel_delete(datamodel_handle::ExosDatamodelHandle)
    @ccall libexos_api.exos_datamodel_delete(
           datamodel_handle::Ref{ExosDatamodelHandle})::EXOS_ERROR_CODE
end

function exos_log_delete(log_handle::ExosLogHandle)
    @ccall libexos_api.exos_log_delete(log_handle::Ref{ExosLogHandle})::EXOS_ERROR_CODE
end

function exos_datamodel_disconnect(datamodel_handle::ExosDatamodelHandle)
    @ccall libexos_api.exos_datamodel_disconnect(
        datamodel_handle::Ref{ExosDatamodelHandle})::EXOS_ERROR_CODE
end

function exos_config_change_cb(log::ExosLogHandle, new_config::ExosLogConfig,
                               user_context::Ptr{Cvoid})
    nothing
end

config_change_cb_c = @cfunction(exos_config_change_cb, Cvoid,
                                (Ref{ExosLogHandle}, Ref{ExosLogConfig},
                                Ptr{Cvoid}))

function exos_string_to_c_ntuple(s::String, max_size::Int64)
    length(codeunits(s)) <= max_size ||
        throw(ArgumentError("String size is too large!"))
    cchar_tuple = ()
    for i in codeunits(s)
        cchar_tuple = tuple(cchar_tuple..., reinterpret(Int8, UInt8(i)))
    end
    # fill with null if there is space left
    if length(cchar_tuple) < max_size
        for i=length(cchar_tuple):max_size-1
            cchar_tuple = tuple(cchar_tuple..., Int8(0))
        end
    end
    cchar_tuple
end
