#include "open62541/build/open62541.h"
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

static volatile UA_Boolean running = true;
int n;
int identificador_variables[5];

/*************************funcion para detener servidor****************************/
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}

/*******************funciones para buscar id de variables del sensor **************************/
static UA_StatusCode nodeIter(UA_NodeId childId, UA_Boolean isInverse, UA_NodeId referenceTypeId, void *handle) {
    if(isInverse)
        return UA_STATUSCODE_GOOD;
    UA_NodeId *parent = (UA_NodeId *)handle;
    n=n+1;
    identificador_variables[n]=childId.identifier.numeric;
    return UA_STATUSCODE_GOOD;
}

static void buscar_id_nodos(UA_Server *server, char *name){
    n=0;   
    UA_NodeId *parent = UA_NodeId_new();
    *parent = UA_NODEID_NUMERIC(1, 1000);
    UA_Server_forEachChildNodeCall(server, UA_NODEID_STRING(1, name),
                                   nodeIter, (void *) parent);
    UA_NodeId_delete(parent);
}

/*******************funcion para crear un nuevo tipo de objetos, Sensor_Type **************************/
static void Definir_Sensor_type(UA_Server *server) {
    //se crea un nuevo tipo de objeto objetType que denominaremos SensorType
    UA_ObjectTypeAttributes stAttr = UA_ObjectTypeAttributes_default;
    stAttr.description = UA_LOCALIZEDTEXT("en-US", "Objeto del tipo sensor");
    stAttr.displayName = UA_LOCALIZEDTEXT("en-US", "SensorType");
    UA_Server_addObjectTypeNode(server, UA_NODEID_NUMERIC(1, 10000),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                UA_QUALIFIEDNAME(1, "SensorType"), stAttr, NULL, NULL); 
    
    //se crea una variable asociada al SensorType que guarda el tiempo de la lectura del sensor  
    UA_VariableAttributes TimeAttr = UA_VariableAttributes_default;
    TimeAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es la marca de tiempo en que se realiza la lectura");
    TimeAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Tiempo_lectura");
    TimeAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_DateTime Tiempo = UA_DateTime_now();
    UA_Variant_setScalar(&TimeAttr.value, &Tiempo, &UA_TYPES[UA_TYPES_DATETIME]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10001),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Tiempo_lectura"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), TimeAttr, NULL, NULL);
    
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10001),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
                            
    //se crea una variable asociada al SensorType que guarda la lectura de temperatura por el metodo directo                          
    UA_VariableAttributes TFDAttr = UA_VariableAttributes_default;
    TFDAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la temperatura por el metodo directo");
    TFDAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "TF_direct");
    TFDAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_TFD = 0;
    UA_Variant_setScalar(&TFDAttr.value, &Valor_TFD, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10002),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "TF_direct"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), TFDAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10002),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);  
    
    //se crea una variable asociada al SensorType que guarda la lectura de temperatura por el metodo de recuperacion espectral                          
    UA_VariableAttributes TFRSAttr = UA_VariableAttributes_default;
    TFRSAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la temperatura por el metodo de recuperación espectral");
    TFRSAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "TF_rec_spect");
    TFRSAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_TFRS = 0;
    UA_Variant_setScalar(&TFRSAttr.value, &Valor_TFRS, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10003),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "TF_rec_spect"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), TFRSAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10003),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);  
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion por temperatura con metodo directo                          
    UA_VariableAttributes RTFDAttr = UA_VariableAttributes_default;
    RTFDAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radicacion global desde temperatura con metodo directo");
    RTFDAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radg_TF_direct");
    RTFDAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RTFD = 0;
    UA_Variant_setScalar(&RTFDAttr.value, &Valor_RTFD, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10004),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radg_TF_direct"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RTFDAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10004),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion por temperatura de recontruccion espectral                         
    UA_VariableAttributes RTFRSAttr = UA_VariableAttributes_default;
    RTFRSAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radicacion global desde temperatura con metodo de recuperación espectral");
    RTFRSAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radg_TF_rec_spect");
    RTFRSAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RTFRS = 0;
    UA_Variant_setScalar(&RTFRSAttr.value, &Valor_RTFRS, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10005),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radg_TF_rec_spect"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RTFRSAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10005),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion con recuperacion espectral                          
    UA_VariableAttributes RRSAttr = UA_VariableAttributes_default;
    RRSAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radiación global por el metodo de recuperación espectral");
    RRSAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radg_rec_spect");
    RRSAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RRS = 0;
    UA_Variant_setScalar(&RRSAttr.value, &Valor_RRS, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10006),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radg_rec_spect"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RRSAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10006),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de radiacion con espectrometro                          
    UA_VariableAttributes RTAttr = UA_VariableAttributes_default;
    RTAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la radiacion total por metodo de area bajo la curva del espectro");
    RTAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Radt");
    RTAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_RT = 0;
    UA_Variant_setScalar(&RTAttr.value, &Valor_RT, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10007),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Radt"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), RTAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10007),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de propension de hollin                           
    UA_VariableAttributes SPAttr = UA_VariableAttributes_default;
    SPAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de propensión de hollin");
    SPAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Soot_propensity");
    SPAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_SP = 0;
    UA_Variant_setScalar(&SPAttr.value, &Valor_SP, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10008),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), SPAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10008),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda el estatus de la camara
    UA_VariableAttributes ECAttr = UA_VariableAttributes_default;
    ECAttr.description =  UA_LOCALIZEDTEXT("en-US", "Estado de la camara false:inactiva-true:activa");
    ECAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Estatus camara");
    ECAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Boolean EC = UA_FALSE;
    UA_Variant_setScalar(&ECAttr.value, &EC, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10009),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Estatus camara"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), ECAttr, NULL, NULL);
                              
      UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10009),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);    
    
    //se crea una variable asociada al SensorType que guarda el estatus del espectrometro
    UA_VariableAttributes EEAttr = UA_VariableAttributes_default;
    EEAttr.description =  UA_LOCALIZEDTEXT("en-US", "Estado del espectrometro false:inactivo-true:activo");
    EEAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "Estatus espectrometro");
    EEAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Boolean EE = UA_FALSE;
    UA_Variant_setScalar(&EEAttr.value, &EE, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10010),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Estatus espectrometro"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), EEAttr, NULL, NULL);
                              
      UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10010),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);    
    
    //se crea una variable asociada al SensorType que guarda la lectura de media de soot propensity                         
    UA_VariableAttributes AVG_STAttr = UA_VariableAttributes_default;
    AVG_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la media de soot propensity");
    AVG_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "avg_soot_propensity");
    AVG_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_AVG_ST = 0;
    UA_Variant_setScalar(&AVG_STAttr.value, &Valor_AVG_ST, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10011),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Avg_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),AVG_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10011),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de mediana de soot propensity                         
    UA_VariableAttributes MEDIAN_STAttr = UA_VariableAttributes_default;
    MEDIAN_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la mediana de soot propensity");
    MEDIAN_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "median_soot_propensity");
    MEDIAN_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_MEDIAN_ST = 0;
    UA_Variant_setScalar(&MEDIAN_STAttr.value, &Valor_MEDIAN_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10012),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Median_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), MEDIAN_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10012),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura de desviacion estandar de soot propensity                         
    UA_VariableAttributes STD_STAttr = UA_VariableAttributes_default;
    STD_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la desviacion estandar de soot propensity");
    STD_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "std_soot_propensity");
    STD_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_STD_ST = 0;
    UA_Variant_setScalar(&STD_STAttr.value, &Valor_STD_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10013),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Std_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), STD_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10013),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    
    //se crea una variable asociada al SensorType que guarda la lectura de la moda de soot propensity                         
    UA_VariableAttributes MODE_STAttr = UA_VariableAttributes_default;
    MODE_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor de la moda de soot propensity");
    MODE_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "mode_soot_propensity");
    MODE_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_MODE_ST = 0;
    UA_Variant_setScalar(&MODE_STAttr.value, &Valor_MODE_ST, &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10014),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Mode_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), MODE_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10014),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
                           
    //se crea una variable asociada al SensorType que guarda la lectura del percentil 50 de soot propensity                         
    UA_VariableAttributes P50_STAttr = UA_VariableAttributes_default;
    P50_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del percentil 50 de soot propensity");
    P50_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "P50_soot_propensity");
    P50_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_P50_ST = 0;
    UA_Variant_setScalar(&P50_STAttr.value, &Valor_P50_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10015),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "P50_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), P50_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10015),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura del segundo momento de soot propensity                         
    UA_VariableAttributes MOM2_STAttr = UA_VariableAttributes_default;
    MOM2_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del segundo momento de soot propensity");
    MOM2_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "MOM2_soot_propensity");
    MOM2_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_MOM2_ST = 0;
    UA_Variant_setScalar(&MOM2_STAttr.value, &Valor_MOM2_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10016),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "MOM2_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), MOM2_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10016),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
                           
    //se crea una variable asociada al SensorType que guarda la lectura del tercer momento de soot propensity                         
    UA_VariableAttributes MOM3_STAttr = UA_VariableAttributes_default;
    MOM3_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del tercer momento de soot propensity");
    MOM3_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "MOM3_soot_propensity");
    MOM3_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_MOM3_ST = 0;
    UA_Variant_setScalar(&MOM3_STAttr.value, &Valor_MOM3_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10017),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "MOM3_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), MOM3_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10017),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura del cuarto momento de soot propensity                         
    UA_VariableAttributes MOM4_STAttr = UA_VariableAttributes_default;
    MOM4_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del cuarto momento de soot propensity");
    MOM4_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "MOM4_soot_propensity");
    MOM4_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_MOM4_ST = 0;
    UA_Variant_setScalar(&MOM4_STAttr.value, &Valor_MOM4_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10018),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "MOM4_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), MOM4_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10018),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura del primer coeficiente de asimetria de soot propensity                         
    UA_VariableAttributes SKEW1_STAttr = UA_VariableAttributes_default;
    SKEW1_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del primer coeficiente de asimetria de soot propensity");
    SKEW1_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "skew1_soot_propensity");
    SKEW1_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_SKEW1_ST = 0;
    UA_Variant_setScalar(&SKEW1_STAttr.value, &Valor_SKEW1_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10019),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Skew1_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), SKEW1_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10019),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura del segundo coeficiente de asimetria de soot propensity                         
    UA_VariableAttributes SKEW2_STAttr = UA_VariableAttributes_default;
    SKEW2_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del segundo coeficiente de asimetria de soot propensity");
    SKEW2_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "skew2_soot_propensity");
    SKEW2_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_SKEW2_ST = 0;
    UA_Variant_setScalar(&SKEW2_STAttr.value, &Valor_SKEW2_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10020),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Skew2_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), SKEW2_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10020),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura del coeficiente de asimetria de soot propensity                         
    UA_VariableAttributes SKEW3_STAttr = UA_VariableAttributes_default;
    SKEW3_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del coeficiente de asimetria de soot propensity");
    SKEW3_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "skew3_soot_propensity");
    SKEW3_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_SKEW3_ST = 0;
    UA_Variant_setScalar(&SKEW3_STAttr.value, &Valor_SKEW3_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10021),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Skew3_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), SKEW3_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10021),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
    //se crea una variable asociada al SensorType que guarda la lectura del coeficiente de curtosis de soot propensity                         
    UA_VariableAttributes KURT_STAttr = UA_VariableAttributes_default;
    KURT_STAttr.description =  UA_LOCALIZEDTEXT("en-US", "Este es el valor del coeficiente de asimetria de soot propensity");
    KURT_STAttr.displayName =  UA_LOCALIZEDTEXT("en-US", "kurt_soot_propensity");
    KURT_STAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_UInt32 Valor_KURT_ST = 0;
    UA_Variant_setScalar(&KURT_STAttr.value, &Valor_KURT_ST , &UA_TYPES[UA_TYPES_UINT32]);
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1, 10022),
                              UA_NODEID_NUMERIC(1, 10000), UA_NODEID_NUMERIC(0, UA_NS0ID_HASPROPERTY),
                              UA_QUALIFIEDNAME(1, "Kurt_soot_propensity"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), KURT_STAttr, NULL, NULL);
                              
    UA_Server_addReference(server, UA_NODEID_NUMERIC(1, 10022),
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true); 
    
}

/*******************funcion para crear un nuevo tipo de objetos, Sensor_Type **************************/
static void Crear_variable_sensor(UA_Server *server, char *name) {    
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.description = UA_LOCALIZEDTEXT("en-US", "Sensor de medición de la planta");
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", name);
    UA_Server_addObjectNode(server, UA_NODEID_STRING(1, name),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, name), UA_NODEID_NUMERIC(1, 10000),
                            oAttr, NULL, NULL);           
   buscar_id_nodos(server,name);
   UA_Variant myVar;
   UA_Variant_init(&myVar);
   
     
   /*********************escribir tiempo de la variable**********************/
   UA_DateTime Tiempo = UA_DateTime_now();
   UA_Variant_setScalarCopy(&myVar, &Tiempo, &UA_TYPES[UA_TYPES_DATETIME]);
   UA_Server_writeValue(server, UA_NODEID_NUMERIC(1, identificador_variables[1]), myVar);
   
   
   /*********************escribir Valor de la variable**********************/
   /*UA_Variant_setScalarCopy(&myVar, &Valor, &UA_TYPES[UA_TYPES_UINT32]);
   UA_Server_writeValue(server, UA_NODEID_NUMERIC(1, identificador_variables[4]), myVar);
   
   /*********************escribir Status de la variable**********************/
   /*UA_Boolean status = UA_FALSE;
   UA_Variant_setScalarCopy(&myVar, &status, &UA_TYPES[UA_TYPES_BOOLEAN]);
   UA_Server_writeValue(server, UA_NODEID_NUMERIC(1, identificador_variables[5]), myVar);*/
}



/****************************************************************/

int main(void) {
	   	signal(SIGINT, stopHandler);//funciones de inicio
    	signal(SIGTERM, stopHandler);//funciones de termino

    	UA_Server *server = UA_Server_new(); //se crea el servidor
    	UA_ServerConfig *config = UA_Server_getConfig(server); //configuracion del servidor
    	UA_ServerConfig_setDefault(config);//definicion de parametros 

    
      Definir_Sensor_type(server);
      Crear_variable_sensor(server, "sensor 10");
	
   
    	 UA_StatusCode retval = UA_Server_run(server, &running);//inicio del servidor


    	UA_Server_delete(server);
    	return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}

