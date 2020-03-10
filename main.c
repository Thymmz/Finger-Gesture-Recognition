#include <stdio.h>
#include <stdlib.h>
#include <pcf/ui/ui.h>
#include <pcf/ui/mainform/mainform.h>
#include <pcf/graphics/graphics.h>
#include <pcf/ui/imageview/imageview.h>
#include <pcf/io/camera/camera.h>
#include <pcf/ui/button/button.h>
#include <pcf/ui/control/control.h>
#include "use.h"

extern pcf_status_t PCF_CAMERA_API pcf_io_camera_showPropertyControl(const IoCamera_t* camera);
extern void sysInit();
void* camMemAlloc(size_t size);
void  camMemFree(void* mem);
void  PCF_STDCALL CameraCaptureHandler(const IoCamera_t* camera, IoCameraImage_t* image);
void  PCF_STDCALL buttonClickHandler(UiButton_t* button);

static UiImageView_t* gs_camview = NULL;
static IoCamera_t* gs_camera;
static UiButton_t* gs_startButton = NULL;
static UiButton_t* gs_propertiesButton = NULL;
static UiButton_t* gs_stopButton = NULL;

static int32_t mainform_padding = 16;
static int32_t mainform_titlebar_padding = 40;
static int32_t camera_image_width = 320;
static int32_t camera_image_height = 240;
static int32_t button_height = 24;
static int32_t button_width = 50;
static int32_t view_offset = 5;

int initCamera();
void addImageView(UiMainForm_t* mainform, int x, int y, uint32_t width, uint32_t height);
void addButtons(UiMainForm_t* mainform, int x, int y);

int main()
{
    sysInit();
    gs_camera = NULL;
    gs_camview = NULL;

    UiControlInfo_t info = {0};
    info.parent = NULL;
    info.typeId = enControlTypeIdValue_Mainform;
    info.location.X = 10;
    info.location.Y = 10;
    info.size.Height = camera_image_height + 2 * view_offset + mainform_titlebar_padding + button_height;
    info.size.Width =  camera_image_width  + 2 * view_offset + mainform_padding;
    info.maximumSize.Height = info.size.Height;
    info.maximumSize.Width  = info.size.Width;
    info.minimumSize.Height = info.size.Height;
    info.minimumSize.Width  = info.size.Width;

    info.text = "Camera View";
    pcf_status_t status;

    if(pcf_ui_isInitialized() &&
       pcf_graphics_isInitialized())
    {
        UiMainForm_t* mainform = pcf_ui_mainform_create(&info, &status);
        if(mainform)
        {
            int32_t exitcode;
            addImageView(mainform, view_offset, view_offset, camera_image_width, camera_image_height);
            if(gs_camview)
            {
                addButtons(mainform, view_offset, camera_image_height + view_offset + 3);
                initCamera();
                if(gs_camera)
                {
                    UiControl_t* btnControl = pcf_ui_button_asControl(gs_startButton);
                    pcf_ui_control_setEnabled(btnControl, enBoolean_True);
                    btnControl = pcf_ui_button_asControl(gs_propertiesButton);
                    pcf_ui_control_setEnabled(btnControl, enBoolean_True);
                    btnControl = pcf_ui_button_asControl(gs_stopButton);
                    pcf_ui_control_setEnabled(btnControl, enBoolean_True);
                }
                status = pcf_ui_mainform_run(mainform, &exitcode);
                if(gs_camera)
                {
                    pcf_io_camera_delete(gs_camera); //Don't forget to delete the camera.
                }
            }
        }
    }
    return 0;
}
void addButtons(UiMainForm_t* mainform, int x, int y)
{
    UiControl_t* parent = pcf_ui_mainform_asControl(mainform);
    if(parent)
    {
        UiControlInfo_t info = {0};
        info.parent = parent;
        info.typeId = enControlTypeIdValue_SimpleButton;
        info.location.X = x;
        info.location.Y = y;
        info.size.Height = button_height;
        info.size.Width = button_width;
        info.text = "Start";

        pcf_status_t status;
        UiButton_t* button = pcf_ui_button_new(&info,&status);
        UiControl_t* btnControl = NULL;
        if(button != NULL)
        {
            gs_startButton = button;
            pcf_ui_button_attachClickedEventHandler(gs_startButton, buttonClickHandler);
            btnControl = pcf_ui_button_asControl(button);
            pcf_ui_control_setEnabled(btnControl, enBoolean_False);
            button = NULL;
            btnControl = NULL;
        }
        int delta = 2*button_width + view_offset;
        button = NULL;
        info.location.X = x + delta;
        info.location.Y = y;
        info.size.Width = button_width + 20;
        info.text = "Properties";
        button = pcf_ui_button_new(&info,&status);
        if(button != NULL)
        {
            gs_propertiesButton = button;
            pcf_ui_button_attachClickedEventHandler(gs_propertiesButton, buttonClickHandler);
            btnControl = pcf_ui_button_asControl(button);
            pcf_ui_control_setEnabled(btnControl, enBoolean_False);
            button = NULL;
            btnControl = NULL;
        }
        delta = 4*button_width + view_offset+20;
        button = NULL;
        info.location.X = x + delta;
        info.location.Y = y;
        info.size.Width = button_width + 20;
        info.text = "Stop";
        button = pcf_ui_button_new(&info,&status);
         if(button != NULL)
        {
            gs_stopButton = button;
            pcf_ui_button_attachClickedEventHandler(gs_stopButton, buttonClickHandler);
            btnControl = pcf_ui_button_asControl(button);
            pcf_ui_control_setEnabled(btnControl, enBoolean_False);
            button = NULL;
            btnControl = NULL;
        }
    }
}

void PCF_STDCALL buttonClickHandler(UiButton_t* button)
{
    if(button == gs_startButton)
    {
        pcf_status_t status = pcf_io_camera_start(gs_camera);
        if(PCF_S_IS_SUCCESS(status))
        {
            UiControl_t* btnControl = pcf_ui_button_asControl(gs_startButton);
            pcf_ui_control_setEnabled(btnControl, enBoolean_False);
            btnControl = pcf_ui_button_asControl(gs_propertiesButton);
            pcf_ui_control_setEnabled(btnControl, enBoolean_True);
            btnControl = pcf_ui_button_asControl(gs_stopButton);
            pcf_ui_control_setEnabled(btnControl, enBoolean_True);
        }
    }
    else if(button == gs_propertiesButton)
    {
        pcf_status_t status = pcf_io_camera_isRunning(gs_camera);
        if(status != PCF_IO_CameraOk)
        {
            pcf_io_camera_showPropertyControl(gs_camera);
            UiControl_t* btnControl = pcf_ui_button_asControl(gs_stopButton);
            pcf_ui_control_setEnabled(btnControl, enBoolean_True);
        }
    }
    else if(button==gs_stopButton)
    {
         pcf_status_t status = pcf_io_camera_stop(gs_camera);
         if(PCF_S_IS_SUCCESS(status))
         {
            
         }
    }

}

void addImageView(UiMainForm_t* mainform, int x, int y, uint32_t width, uint32_t height)
{
    UiControl_t* parent = pcf_ui_mainform_asControl(mainform);
    if(parent)
    {
        UiControlInfo_t info = {0};
        info.parent = parent;
        info.typeId = enControlTypeIdValue_ImageView;
        info.location.X = x;
        info.location.Y = y;
        info.size.Height = height;
        info.size.Width = width;
        info.text = "Image";
        pcf_status_t status;
        UiImageView_t* imview = pcf_ui_imageView_new(&info,&status);
        if(imview == NULL)
        {
            fprintf(stdout, "Failed while creating imview control, status = %d\n", status);
            return;
        }
        gs_camview = imview;
    }
}

int initCamera()
{
    int camera_count;
    pcf_status_t status = pcf_io_camera_enumerate(&camera_count);
    if(!PCF_S_IS_SUCCESS(status))
    {
        return -1;
    }

    int camera_id = camera_count - 1; //this could be negative
    status = pcf_io_camera_canCreateNew(camera_id);
    if(!PCF_S_IS_SUCCESS(status))
    {
        return -1;
    }
    IoCameraInfo_t info;
    status = pcf_io_camera_getCameraInfo(camera_id, &info);
    if(!PCF_S_IS_SUCCESS(status))
    {
        return -1;
    }

    IoCamera_t* camera = pcf_io_camera_new(&info, &status);
    if(camera)
    {
        if(PCF_S_IS_SUCCESS(status))
        {
            IoCameraSetupArgs_t e = {0};
            e.i_cameraId = info.id;
            e.i_captureHandler = CameraCaptureHandler;
            e.i_colorModel = enIoCameraColorModel_RGB24;
            e.i_FrameInterval = 333333;
            e.i_height = 240;
            e.i_width = 320;
            e.i_memAlloc = camMemAlloc;
            e.i_memFree = camMemFree;
            e.i_videoType = enIoCameraVideoType_NTSC_M; //Value is ignored.
            status = pcf_io_camera_setup(camera, &e); //The fields of setup arg get updated, so check them back after successful setup.
            if(!PCF_S_IS_SUCCESS(status))
            {
                pcf_io_camera_delete(camera);
                printf("Failed while setting up the camera\n");
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }
    gs_camera = camera;
    return 0;
}

void PCF_STDCALL CameraCaptureHandler(const IoCamera_t* camera, IoCameraImage_t* image)
{
    if(image != NULL && camera == gs_camera)
    {
        printf("Time stamp = %f, BufferSize = %u\n", image->timeStamp, image->length);
        if(gs_camview != NULL)
        {
            pcf_ui_imageView_showRGB24(gs_camview, image->pixels, image->width, image->height);
        }
    }
    if(image)
    {
        pcf_io_camera_save(image, "image.bmp");
        
    }
    imageinput(image);
        // status = pcf_io_camera_delete(camera);
        // if(PCF_S_IS_SUCCESS(status))
        // {
        //     printf("Successfully deleted camera\n");
        // }
}

void* camMemAlloc(size_t size)
{
    return malloc(size);
}

void camMemFree(void* mem)
{
    free(mem);
    return;
}
