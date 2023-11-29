module EXOS

using Match
import Base. +

libexos_api = "./libexos-api.so"

const EXOS_ARRAY_DEPTH = 10
const EXOS_LOG_EXCLUDE_LIST_LEN = 20
const EXOS_LOG_MAX_NAME_LENGTH  = 35
const EXOS_LOG_MESSAGE_LENGTH   = 256

# Macro that exports all instances in an enum.
macro export_enum(T)
    return esc(quote
        export $T
        for inst in Symbol.(instances($T))
            eval($(Expr(:quote, :(export $(Expr(:$, :inst))))))
        end
    end)
end

export
    EXOS_ARRAY_DEPTH,
    EXOS_LOG_EXCLUDE_LIST_LEN,
    EXOS_LOG_MAX_NAME_LENGTH,
    EXOS_LOG_MESSAGE_LENGTH,
    exos_set_path
    +(enum1::Enum, enum2::Enum) = Base.UInt32(enum1) + Base.UInt32(enum2)

function exos_set_path(path::String)
    global libexos_api = path
end

include("enums.jl")
@export_enum EXOS_ERROR_CODE
@export_enum EXOS_CONNECTION_STATE
@export_enum EXOS_DATAMODEL_EVENT_TYPE
@export_enum EXOS_DATAMODEL_PROCESS_MODE
@export_enum EXOS_DATASET_TYPE
@export_enum EXOS_DATASET_EVENT_TYPE
@export_enum EXOS_LOG_LEVEL
@export_enum EXOS_LOG_TYPE

include("types.jl")
export
    ExosDatasetInfo,
    JExosDatasetInfo,
    ExosDatamodelSyncInfo,
    JExosDatamodelSyncInfo,
    ExosDatamodelPrivate,
    JExosDatamodelPrivate,
    ExosDatamodelHandle,
    JExosDatamodelHandle,
    ExosBufferInfo,
    JExosBufferInfo,
    ExosDatasetPrivate,
    JExosDatasetPrivate,
    ExosDatasetHandle,
    JExosDatasetHandle,
    ExosLogPrivate,
    JExosLogPrivate,
    ExosLogHandle,
    JExosLogHandle,
    ExosLogConfigType,
    JExosLogConfigType,
    ExosLogConfig,
    JExosLogConfig

include("api.jl")
export
    exos_datamodel_calc_dataset_info,
    exos_datamodel_init,
    exos_dataset_init,
    exos_datamodel_connect,
    exos_dataset_connect,
    exos_datamodel_process,
    exos_dataset_publish,
    exos_log_init,
    exos_log_info,
    exos_log_process,
    exos_datamodel_delete,
    exos_log_delete,
    exos_config_change_cb,
    config_change_cb_c,
    exos_datamodel_disconnect,
    exos_string_to_c_ntuple

end # EXOS
