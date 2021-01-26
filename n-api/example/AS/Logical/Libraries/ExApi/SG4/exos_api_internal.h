#ifndef EXOS_API_INTERNAL_H
#define EXOS_API_INTERNAL_H

#include "exos_api.h"

#define EXOS_ARRAY_DEPTH 10

typedef struct exos_dataset_info
{
    const char *name;
    void *adr;
    size_t size;
    long offset;
    uint32_t arrayItems[EXOS_ARRAY_DEPTH];

} exos_dataset_info_t;

#define EXOS_DATASET_BROWSE_NAME_INIT "", &(data), sizeof(data), 0
#define EXOS_DATASET_BROWSE_NAME(_arg_) #_arg_, &(data._arg_), sizeof(data._arg_), 0

char *_exos_internal_parse_config(const char *name, const char *config, const exos_dataset_info_t *info, size_t info_size);
EXOS_ERROR_CODE _exos_internal_datamodel_connect(exos_datamodel_handle_t *artefact, const char *config, const exos_dataset_info_t *info, size_t info_size, exos_datamodel_event_cb datamodel_event_callback);
void _exos_internal_calc_offsets(exos_dataset_info_t *info, size_t info_size);

#endif // EXOS_API_INTERNAL_H
