// FSAVP.cpp

#define VDW_MAIN
#include <vdw.h>
#include "function.h"
#include "FSAVP.h"
#include "msglog.h"
// Include files for device classes
#include "FSAVPDev.h"
#include "FSBase.h"




// Generated by Driver::Wizard version 1.20

// Defaults and globals used for tracing
	ULONG		defTraceDevice = TRACE_MONITOR | TRACE_DEBUGGER;// Default trace device
	TRACE_LEVEL defTraceOutputLevel = TraceAlways;              // Default trace output level
	BREAK_LEVEL defTraceBreakLevel = BreakNever;                // Default trace break level
#ifdef _TRACER
	KTrace		Tracer("FSAVP");	// Global driver trace object
#endif
//	KTrace		*Tracer;	// Global driver trace object


//	Unload is responsible for releasing any system objects that
//	the driver has allocated. 
//
//	In general, this function must comprehensively ensure that
//	the driver is not unloaded while holding system objects,
//	including memory, or while there are pending events that
//	would cause the system to call the driver. This is best done
//	by deconstructing top level objects, which in turn release
//	objects for which they are responsible.
//
//	This function is called at PASSIVE_LEVEL.
//
#pragma code_seg("INIT")

DECLARE_DRIVER_CLASS(FSAVP, NULL)

// Driver Entry routine
//		This routine is called when the driver is loaded.
//		Usually, this is where any devices associated with
//		the driver are created.
//
//		The driver often reads the registry at DriverEntry in
//		order to setup various configurable parameters.
//		
//		Driver::Works makes it easy to use the registry to also
//		control what devices are present and should be created.
//
NTSTATUS FSAVP::DriverEntry(PUNICODE_STRING RegistryPath)
{
	NTSTATUS status=STATUS_INSUFFICIENT_RESOURCES;
//return status;
	// Status of device creation
//	DbgPrint("FSAVP: DriverEntry\n");
	SetPoolTag(' wen');			// Set default pool tag for all 'new' allocations
								// Under checked build, use POOLMON to view tags

	// Automatically load following variables from registry values
	// The default value is preloaded.  If the appropriate registry
	// value is not found, the default will be left unchanged.

	// Open the driver "Parameters" key
	pFSAVPDriver=this;
	sRegistryPath.Assign(L"");
	sRegistryPath.Append(RegistryPath->Buffer);
	KRegistryKey Params(sRegistryPath, L"Parameters");
	if (!NT_SUCCESS(Params.LastError()))
	{
		// Error, could not open registry key
		return STATUS_INSUFFICIENT_RESOURCES;
	}


#ifdef _TRACER
	Params.QueryValue(L"TraceDevice", (PULONG) &defTraceDevice);
	Params.QueryValue(L"TraceOutputLevel", (PULONG) &defTraceOutputLevel);
	Params.QueryValue(L"TraceBreakLevel", (PULONG) &defTraceBreakLevel);

	Tracer.SetTarget(defTraceDevice);
	Tracer.SetOutputLevel(defTraceOutputLevel);
	Tracer.SetBreakLevel(defTraceBreakLevel);
#endif

//#endif


// Create FSAVPDevice

	FSAVPDevice* pFSAVPDevice;
	pFSAVPDevice = new (NonPagedPool) FSAVPDevice();
	if (pFSAVPDevice == NULL)
	{
		_DebugTrace(TraceError, "Error constructing device FSAVPDevice\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	status = pFSAVPDevice->ConstructorStatus();
	if (!NT_SUCCESS(status))
	{
		// Error returned from a constructor
		_DebugTrace(TraceError, "Error creating device FSAVPDevice, status %x\n", status);
		delete pFSAVPDevice;
		return status;
	}
	
	status=BaseInit();
	if(!NT_SUCCESS(status))
    {
		_DebugTrace(TraceError, "Failed to INIT");
		BaseDestroy();
		delete pFSAVPDevice;
    }

	return status;
}


#pragma code_seg()

VOID FSAVP::Unload(VOID)
{

   // If you don't need to perform any functions
   // except to call the base class KDriver::Unload(),
   // then this entire routine may be safely deleted.

    // Call base class to delete all devices.
	_DebugTrace(TraceInfo, "Unload called\n");
	BaseDestroy();
	KDriver::Unload();
//	delete Tracer;
}
