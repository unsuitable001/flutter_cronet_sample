#include"dart_api.h"
#include"dart_native_api.h"
#include"dart_tools_api.h"
// #include"dart_api_dl.c"
#include"wrapper.h"
#include "sample_executor.h"
#include <iostream>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////
// Initialize `dart_api_dl.h`
intptr_t InitDartApiDL(void* data) {
  printf("Initializing");
  return Dart_InitializeApiDL(data);
}

////////////////////////////////////////////////////////////////////////////////

// loading cronet
void *handle = dlopen("libcronet.91.0.4456.0.so", RTLD_NOW);
Dart_Port _callback_port;
SampleExecutor executor;
void destroy() {
    dlclose(handle);
}

static void FreeFinalizer(void*, void* value) {
  free(value);
}


/* Callback Helpers */

void registerCallbackHandler(Dart_Port send_port) {_callback_port = send_port;}
void dispatchCallback(char* methodname, Dart_CObject args) {

  Dart_CObject c_method_name;
  c_method_name.type = Dart_CObject_kString;
  c_method_name.value.as_string = methodname;

  Dart_CObject* c_request_arr[] = {&c_method_name, &args};
  Dart_CObject c_request;

  c_request.type = Dart_CObject_kArray;
  c_request.value.as_array.values = c_request_arr;
  c_request.value.as_array.length =
      sizeof(c_request_arr) / sizeof(c_request_arr[0]);
  
  Dart_PostCObject_DL(_callback_port, &c_request);
}

Dart_CObject callbackArgBuilder(int num, ...) {
  Dart_CObject c_request_data;
  va_list valist;
  va_start(valist, num);
  void* request_buffer = malloc(sizeof(uint64_t) * num);
  uint64_t* buf =  reinterpret_cast<uint64_t*>(request_buffer);

  for(int i = 0; i < num; i++) {
    buf[i] = va_arg(valist,uint64_t);
  }

  c_request_data.type = Dart_CObject_kExternalTypedData;
  c_request_data.value.as_external_typed_data.type = Dart_TypedData_kUint64;
  c_request_data.value.as_external_typed_data.length = sizeof(uint64_t) * num;  // 4 args to pass
  c_request_data.value.as_external_typed_data.data = static_cast<uint8_t*>(request_buffer);
  c_request_data.value.as_external_typed_data.peer = request_buffer;
  c_request_data.value.as_external_typed_data.callback = FreeFinalizer;

  va_end(valist);

  return c_request_data;
}


/* Getting Cronet's Functions */

Cronet_EnginePtr (*_Cronet_Engine_Create)(void) = reinterpret_cast<Cronet_EnginePtr (*)(void)>(dlsym(handle,"Cronet_Engine_Create"));
Cronet_String (*_Cronet_Engine_GetVersionString)(Cronet_EnginePtr) = reinterpret_cast<Cronet_String (*)(Cronet_EnginePtr)>(dlsym(handle,"Cronet_Engine_GetVersionString"));
Cronet_EngineParamsPtr (*_Cronet_EngineParams_Create)(void) = reinterpret_cast<Cronet_EngineParamsPtr (*)(void)>(dlsym(handle,"Cronet_EngineParams_Create"));
void (*_Cronet_EngineParams_Destroy)(Cronet_EngineParamsPtr) = reinterpret_cast<void (*)(Cronet_EngineParamsPtr)>(dlsym(handle,"Cronet_EngineParams_Destroy"));
void (*_Cronet_EngineParams_enable_check_result_set)(Cronet_EngineParamsPtr, const bool) = reinterpret_cast<void (*)(Cronet_EngineParamsPtr, const bool)>(dlsym(handle,"Cronet_EngineParams_enable_check_result_set"));
void (*_Cronet_EngineParams_user_agent_set)(Cronet_EngineParamsPtr, const Cronet_String) = reinterpret_cast<void (*)(Cronet_EngineParamsPtr, const Cronet_String)>(dlsym(handle,"Cronet_EngineParams_enable_check_result_set"));
void (*_Cronet_EngineParams_enable_quic_set)(Cronet_EngineParamsPtr, const bool) = reinterpret_cast<void (*)(Cronet_EngineParamsPtr, const bool)>(dlsym(handle,"Cronet_EngineParams_enable_quic_set"));
Cronet_RESULT (*_Cronet_Engine_StartWithParams)(Cronet_EnginePtr, Cronet_EngineParamsPtr) = reinterpret_cast<Cronet_RESULT (*)(Cronet_EnginePtr, Cronet_EngineParamsPtr)>(dlsym(handle,"Cronet_Engine_StartWithParams"));

Cronet_UrlRequestPtr (*_Cronet_UrlRequest_Create)(void) = reinterpret_cast<Cronet_UrlRequestPtr (*)(void)>(dlsym(handle,"Cronet_UrlRequest_Create"));
void (*_Cronet_UrlRequest_Destroy)(Cronet_UrlRequestPtr) = reinterpret_cast<void (*)(Cronet_UrlRequestPtr)>(dlsym(handle,"Cronet_UrlRequest_Destroy"));
void (*_Cronet_UrlRequest_SetClientContext) (Cronet_UrlRequestPtr, Cronet_ClientContext) = reinterpret_cast<void (*) (Cronet_UrlRequestPtr, Cronet_ClientContext)>(dlsym(handle,"Cronet_UrlRequest_SetClientContext"));
Cronet_ClientContext (*_Cronet_UrlRequest_GetClientContext)(Cronet_UrlRequestPtr) = reinterpret_cast<Cronet_ClientContext (*)(Cronet_UrlRequestPtr)>(dlsym(handle,"Cronet_UrlRequest_GetClientContext"));


Cronet_UrlRequestParamsPtr (*_Cronet_UrlRequestParams_Create)(void)  = reinterpret_cast<Cronet_UrlRequestParamsPtr (*)(void)>(dlsym(handle,"Cronet_UrlRequestParams_Create"));
void (*_Cronet_UrlRequestParams_http_method_set)(
    Cronet_UrlRequestParamsPtr,
    const Cronet_String) = reinterpret_cast<void (*)(Cronet_UrlRequestParamsPtr,
                                             const Cronet_String)>(dlsym(handle,"Cronet_UrlRequestParams_http_method_set"));



// Unexposed - see Cronet_UrlRequest_Init
Cronet_RESULT (*_Cronet_UrlRequest_InitWithParams)(
    Cronet_UrlRequestPtr,
    Cronet_EnginePtr,
    Cronet_String,
    Cronet_UrlRequestParamsPtr,
    Cronet_UrlRequestCallbackPtr,
    Cronet_ExecutorPtr) = reinterpret_cast<Cronet_RESULT (*)(
    Cronet_UrlRequestPtr,
    Cronet_EnginePtr,
    Cronet_String,
    Cronet_UrlRequestParamsPtr,
    Cronet_UrlRequestCallbackPtr,
    Cronet_ExecutorPtr)>(dlsym(handle,"Cronet_UrlRequest_InitWithParams"));

// Unexposed - see Cronet_UrlRequest_Init
Cronet_UrlRequestCallbackPtr (*_Cronet_UrlRequestCallback_CreateWith)(Cronet_UrlRequestCallback_OnRedirectReceivedFunc,
    Cronet_UrlRequestCallback_OnResponseStartedFunc,
    Cronet_UrlRequestCallback_OnReadCompletedFunc,
    Cronet_UrlRequestCallback_OnSucceededFunc,
    Cronet_UrlRequestCallback_OnFailedFunc,
    Cronet_UrlRequestCallback_OnCanceledFunc) = reinterpret_cast<Cronet_UrlRequestCallbackPtr (*)(Cronet_UrlRequestCallback_OnRedirectReceivedFunc,
    Cronet_UrlRequestCallback_OnResponseStartedFunc,
    Cronet_UrlRequestCallback_OnReadCompletedFunc,
    Cronet_UrlRequestCallback_OnSucceededFunc,
    Cronet_UrlRequestCallback_OnFailedFunc,
    Cronet_UrlRequestCallback_OnCanceledFunc)>(dlsym(handle,"Cronet_UrlRequestCallback_CreateWith"));

Cronet_RESULT (*_Cronet_UrlRequest_Start)(Cronet_UrlRequestPtr) = reinterpret_cast<Cronet_RESULT (*)(Cronet_UrlRequestPtr)>(dlsym(handle,"Cronet_UrlRequest_Start"));
Cronet_RESULT (*_Cronet_UrlRequest_FollowRedirect)(Cronet_UrlRequestPtr) = reinterpret_cast<Cronet_RESULT (*)(Cronet_UrlRequestPtr)>(dlsym(handle,"Cronet_UrlRequest_FollowRedirect"));
Cronet_RESULT (*_Cronet_UrlRequest_Read)(Cronet_UrlRequestPtr, Cronet_BufferPtr) = reinterpret_cast<Cronet_RESULT (*)(Cronet_UrlRequestPtr, Cronet_BufferPtr)>(dlsym(handle,"Cronet_UrlRequest_Read"));

Cronet_BufferPtr (*_Cronet_Buffer_Create)(void) = reinterpret_cast<Cronet_BufferPtr (*)(void)>(dlsym(handle,"Cronet_Buffer_Create"));
void (*_Cronet_Buffer_Destroy)(Cronet_BufferPtr) = reinterpret_cast<void (*)(Cronet_BufferPtr)>(dlsym(handle,"Cronet_Buffer_Destroy"));
void (*_Cronet_Buffer_InitWithAlloc)(Cronet_BufferPtr, uint64_t) = reinterpret_cast<void (*)(Cronet_BufferPtr, uint64_t)>(dlsym(handle,"Cronet_Buffer_InitWithAlloc"));
uint64_t (*_Cronet_Buffer_GetSize)(Cronet_BufferPtr) = reinterpret_cast<uint64_t (*)(Cronet_BufferPtr)>(dlsym(handle,"Cronet_Buffer_GetSize"));
Cronet_RawDataPtr (*_Cronet_Buffer_GetData)(Cronet_BufferPtr) = reinterpret_cast<Cronet_RawDataPtr (*)(Cronet_BufferPtr)>(dlsym(handle,"Cronet_Buffer_GetData"));

Cronet_String (*_Cronet_Error_message_get)(const Cronet_ErrorPtr) = reinterpret_cast<Cronet_String (*)(const Cronet_ErrorPtr)>(dlsym(handle,"Cronet_Error_message_get"));

/* URL Callbacks Implementations */

void OnRedirectReceived(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info,
    Cronet_String newLocationUrl) {
    dispatchCallback("OnRedirectReceived", callbackArgBuilder(1, newLocationUrl));
  Cronet_UrlRequest_FollowRedirect(request);
}

void OnResponseStarted(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info) {
    
    dispatchCallback("OnResponseStarted", callbackArgBuilder(0));

  // Create and allocate 32kb buffer.
  Cronet_BufferPtr buffer = _Cronet_Buffer_Create();
  _Cronet_Buffer_InitWithAlloc(buffer, 32 * 1024);
  // Started reading the response.
  _Cronet_UrlRequest_Read(request, buffer);
   
}


void OnReadCompleted(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info,
    Cronet_BufferPtr buffer,
    uint64_t bytes_read) {
    dispatchCallback("OnReadCompleted", callbackArgBuilder(4, request, info, buffer, bytes_read));
}


void OnSucceeded(Cronet_UrlRequestCallbackPtr self, Cronet_UrlRequestPtr request, Cronet_UrlResponseInfoPtr info) {
  printf("OnSucceeded");
  std::cout << "OnSucceeded called." << std::endl;
  dispatchCallback("OnSucceeded", callbackArgBuilder(0));
}

void OnFailed(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info,
    Cronet_ErrorPtr error) {
      printf("OnFailed");
  printf("%s",_Cronet_Error_message_get(error));
  dispatchCallback("OnFailed", callbackArgBuilder(1, error));
//   last_error_message_ = Cronet_Error_message_get(error);
}

void OnCanceled(
    Cronet_UrlRequestCallbackPtr self,
    Cronet_UrlRequestPtr request,
    Cronet_UrlResponseInfoPtr info) {
      printf("OnCanceled");
      dispatchCallback("OnFailed", callbackArgBuilder(0));
}


/* Interface */

Cronet_EnginePtr Cronet_Engine_Create() {
  printf("Cronet_Engine_Create");
  if(!handle) {
    std::cout << dlerror();
  }
  return _Cronet_Engine_Create();
}
Cronet_String Cronet_Engine_GetVersionString(Cronet_EnginePtr ce) {
  return _Cronet_Engine_GetVersionString(ce);
}
Cronet_EngineParamsPtr Cronet_EngineParams_Create(void) {return _Cronet_EngineParams_Create();}
void Cronet_EngineParams_Destroy(Cronet_EngineParamsPtr self) {}
void Cronet_EngineParams_enable_check_result_set(
    Cronet_EngineParamsPtr self,
    const bool enable_check_result) {return _Cronet_EngineParams_enable_check_result_set(self,enable_check_result);}

void Cronet_EngineParams_user_agent_set(Cronet_EngineParamsPtr self,
                                        const Cronet_String user_agent){_Cronet_EngineParams_user_agent_set(self, user_agent);}

void Cronet_EngineParams_enable_quic_set(Cronet_EngineParamsPtr self,
                                         const bool enable_quic){_Cronet_EngineParams_enable_quic_set(self,enable_quic);}

Cronet_RESULT Cronet_Engine_StartWithParams(Cronet_EnginePtr self,
                                            Cronet_EngineParamsPtr params) {return _Cronet_Engine_StartWithParams(self, params);}

Cronet_UrlRequestPtr Cronet_UrlRequest_Create(void) {return _Cronet_UrlRequest_Create();}

void Cronet_UrlRequest_Destroy(Cronet_UrlRequestPtr self) {return _Cronet_UrlRequest_Destroy(self);}

void Cronet_UrlRequest_SetClientContext(Cronet_UrlRequestPtr self, Cronet_ClientContext client_context) {return _Cronet_UrlRequest_SetClientContext(self, client_context);}
Cronet_ClientContext Cronet_UrlRequest_GetClientContext(Cronet_UrlRequestPtr self) {return _Cronet_UrlRequest_GetClientContext(self);}

Cronet_UrlRequestParamsPtr Cronet_UrlRequestParams_Create(void) {return _Cronet_UrlRequestParams_Create();}

void Cronet_UrlRequestParams_http_method_set(Cronet_UrlRequestParamsPtr self, const Cronet_String http_method) {return _Cronet_UrlRequestParams_http_method_set(self, http_method);}

Cronet_RESULT Cronet_UrlRequest_Start(Cronet_UrlRequestPtr self) {return _Cronet_UrlRequest_Start(self);}
Cronet_RESULT Cronet_UrlRequest_FollowRedirect(Cronet_UrlRequestPtr self) {return _Cronet_UrlRequest_FollowRedirect(self);}
Cronet_RESULT Cronet_UrlRequest_Read(Cronet_UrlRequestPtr self, Cronet_BufferPtr buffer) {return _Cronet_UrlRequest_Read(self, buffer);}

Cronet_RawDataPtr Cronet_Buffer_GetData(Cronet_BufferPtr buffer) {return _Cronet_Buffer_GetData(buffer);}
uint64_t Cronet_Buffer_GetSize(Cronet_BufferPtr self) {return _Cronet_Buffer_GetSize(self);}





// NOTE: Changed from original cronet's api. executor & callback params aren't needed
Cronet_RESULT Cronet_UrlRequest_Init(Cronet_UrlRequestPtr self, Cronet_EnginePtr engine, Cronet_String url, Cronet_UrlRequestParamsPtr params) {
    executor.Init();
    Cronet_UrlRequestCallbackPtr urCallback = _Cronet_UrlRequestCallback_CreateWith(OnRedirectReceived, OnResponseStarted, OnReadCompleted,
        OnSucceeded, OnFailed, OnCanceled);
    
    return _Cronet_UrlRequest_InitWithParams(self, engine, url, params, urCallback, executor.GetExecutor());

} 

