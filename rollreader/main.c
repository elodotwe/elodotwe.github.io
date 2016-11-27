#include "cv.h" //openCV
#include "highgui.h" //openCV
#include <gtk/gtk.h> //GTK+ is the GUI with the spinner controls and checkbox on it
#include <smf.h> //libsmf handles Simple MIDI format writing

int initUI();
#define WINDOW_NAME "edges"
GtkSpinButton* currentFrameW = 0;
GtkSpinButton* startFrameW = 0;
GtkSpinButton* stopFrameW = 0;
GtkSpinButton* leftXW = 0;
GtkSpinButton* leftYW = 0;
GtkSpinButton* rightXW = 0;
GtkSpinButton* rightYW = 0;
GtkSpinButton* ROIsizeW = 0;
GtkSpinButton* thresholdValueW = 0;

gdouble currentFrame = 0;
gdouble frameCount = 0;
gdouble startFrame = 0;
gdouble stopFrame = 0;
gdouble leftX = 0;
gdouble leftY = 0;
gdouble rightX = 0;
gdouble rightY = 0;
gdouble ROIsize = 0;
gdouble thresholdValue = 0;
gboolean showPoints = FALSE;

gdouble width = 0;
gdouble height = 0;

#define NUMBER_OF_KEYS 85
CvScalar avgs[NUMBER_OF_KEYS];
gboolean noteOn[NUMBER_OF_KEYS] = { FALSE };

CvCapture* cap;

int main()
{
    cap = cvCreateFileCapture( "MVI_3812.AVI" );
    if(!cap)  // check if we succeeded
        return -1;
    cvNamedWindow(WINDOW_NAME,1);

    // Used as a cap for the current frame spin button.
    frameCount = cvGetCaptureProperty( cap, CV_CAP_PROP_FRAME_COUNT );
    width = cvGetCaptureProperty( cap, CV_CAP_PROP_FRAME_WIDTH );
    height = cvGetCaptureProperty( cap, CV_CAP_PROP_FRAME_HEIGHT );

    initUI();

    return 0;
}

void redrawFrame()
{
    // Since QueryFrame increments the frame position, move the frame counter
    // back to the current frame.
    cvSetCaptureProperty( cap, CV_CAP_PROP_POS_FRAMES, currentFrame );
    IplImage* frame = cvQueryFrame( cap );

    gdouble xSpacing = 0.0;
    gdouble ySpacing = 0.0;
    int i = 0;
    CvPoint pt1, pt2;

    int lastChange = 0;

    xSpacing = (rightX - leftX) / NUMBER_OF_KEYS;
    ySpacing = (rightY - leftY) / NUMBER_OF_KEYS;
    for ( i = 0; i < NUMBER_OF_KEYS; i++ )
    {
        // Set pt1 to center of point.
        pt1.x = leftX + ( i * xSpacing );
        pt1.y = leftY + ( i * ySpacing );
        // Adjust for ROI width.
        pt2.x = pt1.x + (ROIsize / 2.0);
        pt2.y = pt1.y + (ROIsize / 2.0);
        pt1.x = pt1.x - (ROIsize / 2.0);
        pt1.y = pt1.y - (ROIsize / 2.0);

        cvSetImageROI( frame, cvRect( pt1.x, pt1.y, ROIsize, ROIsize ) );
        avgs[i] = cvAvg( frame, NULL );

        cvResetImageROI( frame );

        // "I think" I'm looking at the green value for the pixel. Looked "good enough".
        // There's certainly a better way to do this.
        noteOn[i] = (avgs[i].val[1] < thresholdValue);

        if (noteOn[i])
        {
            // Since the note is on, paint a red box around the proper note.
            cvRectangle(frame, pt1, pt2, CV_RGB(255,0,0),1,8,0);
        }
        else
        {
            // The note is off. Paint a green box around the proper note.
            cvRectangle(frame, pt1, pt2, CV_RGB(0,255,0),1,8,0);
        }
    }
    cvShowImage(WINDOW_NAME, frame);
}

G_MODULE_EXPORT void
on_currentFrame_value_changed( GtkSpinButton *button,
               void *      data )
{
    gdouble val = 0.0;
    currentFrame = gtk_spin_button_get_value(button);
    // Move the video to the new current frame; redraw it.
    redrawFrame();
}

G_MODULE_EXPORT void
on_startFrame_value_changed( GtkSpinButton *button,
               void *      data )
{
    // start frame changed; no redraw needed.
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    startFrame = val;
}

G_MODULE_EXPORT void
on_stopFrame_value_changed( GtkSpinButton *button,
               void *      data )
{
    // stop frame changed; no redraw needed.
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    stopFrame = val;
}

G_MODULE_EXPORT void
on_leftX_value_changed( GtkSpinButton *button,
               void *      data )
{
    // left X changed. Redraw needed if points are displayed.
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    leftX = val;
    if (showPoints)
    {
        redrawFrame();
    }
}

G_MODULE_EXPORT void
on_leftY_value_changed( GtkSpinButton *button,
               void *      data )
{
    // left Y changed. Redraw needed if points are displayed.
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    leftY = val;
    if (showPoints)
    {
        redrawFrame();
    }
}

G_MODULE_EXPORT void
on_rightX_value_changed( GtkSpinButton *button,
               void *      data )
{
    // right X changed. Redraw needed if points are displayed.
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    rightX = val;
    if (showPoints)
    {
        redrawFrame();
    }
}

G_MODULE_EXPORT void
on_rightY_value_changed( GtkSpinButton *button,
               void *      data )
{
    // right Y changed. Redraw needed if points are displayed.
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    rightY = val;
    if (showPoints)
    {
        redrawFrame();
    }
}

G_MODULE_EXPORT void
on_boxSize_value_changed( GtkSpinButton *button,
               void *      data )
{
    // ROI size changed. Redraw needed if points are displayed.
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    ROIsize = val;
    if (showPoints)
    {
        redrawFrame();
    }
}

G_MODULE_EXPORT void
on_threshold_value_changed( GtkSpinButton *button,
               void *      data )
{
    // threshold value changed
    gdouble val = 0.0;
    val = gtk_spin_button_get_value(button);
    thresholdValue = val;
    redrawFrame();
}

G_MODULE_EXPORT void
on_showPoints_toggled( GtkToggleButton *button,
               gpointer      data )
{
    // Show points toggled. Redraw will be needed either way.
    showPoints = gtk_toggle_button_get_active(button);
    redrawFrame();
}

G_MODULE_EXPORT void
on_doIt_clicked( GtkButton *button,
               gpointer      data )
{
    printf("button clicked\n");

    smf_t* smf = smf_new();
    smf_track_t* track = smf_track_new();
    smf_add_track(smf, track);

    smf_event_t* event;

    gboolean prevFrame[NUMBER_OF_KEYS] = { FALSE };
    
    // Start video from beginning
    currentFrame = startFrame;

//upper 9 notes have lyrics
//2920 start
    while (currentFrame <= stopFrame)
    {
        int i = 0;
        if ( currentFrame == stopFrame )
        {
            for (i = 0; i < NUMBER_OF_KEYS; i++ )
            {
                noteOn[i] = FALSE;
            }
        }
        else
        {
            redrawFrame();
        }
        // Why NUMBER_OF_KEYS - 10? Well, the lyrics in my example are printed across
        // the upper 10 notes and were being detected as notes themselves, causing odd
        // trilling when they scrolled past. This was a quick hack to make that stop.
        for ( i = 0; i < NUMBER_OF_KEYS - 10; i++ )
        {
            char note = 0xd + i;
            char eventBytes[3] = { 0 };
            eventBytes[1] = note; // which note to play
            eventBytes[2] = 0x7f; // full velocity (may toy with later)
            if (prevFrame[i] && (!noteOn[i]))
            {
                // Time to generate a note-off
                eventBytes[0] = 0x80;
                printf("note-off: %x, at frame %f\n", i + 0xd, currentFrame);
            }
            if ((!prevFrame[i]) && noteOn[i])
            {
                // Time to generate a note-on
                eventBytes[0] = 0x90;
                printf("note-on: %x, at frame %f\n", i + 0xd, currentFrame);
            }

            prevFrame[i] = noteOn[i];

            if ( eventBytes[0] )
            {
                event = smf_event_new_from_pointer(eventBytes, 3);
                smf_track_add_event_seconds(track, event, currentFrame / 30.0);
            }
        }
        currentFrame++;
        gtk_main_iteration_do( FALSE );
    }
    smf_save(smf, "output.mid");
}

G_MODULE_EXPORT gboolean
on_window_delete_event( GtkWidget *widget,
               GdkEvent      *event,
               gpointer      data )
{
    return FALSE;
}

G_MODULE_EXPORT gboolean
on_window_destroy( GtkObject *object,
               gpointer      data )
{
    gtk_main_quit();
}

int initUI()
{
    GtkBuilder              *builder;
    GtkWidget               *window;
        
    gtk_init (NULL,NULL);

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "ui/player-piano.glade", NULL);

    window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
    gtk_builder_connect_signals (builder, NULL);          

    gtk_widget_show (window);       
    currentFrameW = (GtkSpinButton*)gtk_builder_get_object( builder, "currentFrame" );
    leftXW = (GtkSpinButton*)gtk_builder_get_object( builder, "leftX" );
    rightXW = (GtkSpinButton*)gtk_builder_get_object( builder, "rightX" );
    leftYW = (GtkSpinButton*)gtk_builder_get_object( builder, "leftY" );
    rightYW = (GtkSpinButton*)gtk_builder_get_object( builder, "rightY" );
    ROIsizeW = (GtkSpinButton*)gtk_builder_get_object( builder, "boxSize" );
    thresholdValueW = (GtkSpinButton*)gtk_builder_get_object( builder, "threshold" );
    startFrameW = (GtkSpinButton*)gtk_builder_get_object( builder, "startFrame" );
    stopFrameW = (GtkSpinButton*)gtk_builder_get_object( builder, "stopFrame" );
#pragma message("TODO: this is disgustingly verbose. Clean me up.")
    GtkObject* frameCountAdj = 0;
    frameCountAdj = gtk_adjustment_new(0, 0, frameCount-1, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)currentFrameW, (GtkAdjustment*)frameCountAdj, 1, 0);

    GtkObject* startFrameAdj = 0;
    startFrameAdj = gtk_adjustment_new(0, 0, frameCount-1, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)startFrameW, (GtkAdjustment*)startFrameAdj, 1, 0);

    GtkObject* stopFrameAdj = 0;
    stopFrameAdj = gtk_adjustment_new(0, 0, frameCount-1, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)stopFrameW, (GtkAdjustment*)stopFrameAdj, 1, 0);

    GtkObject* leftWidthAdj = 0;
    leftWidthAdj = gtk_adjustment_new(0, 0, width-1, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)leftXW, (GtkAdjustment*)leftWidthAdj, 1, 0);

    GtkObject* rightWidthAdj = 0;
    rightWidthAdj = gtk_adjustment_new(0, 0, width-1, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)rightXW, (GtkAdjustment*)rightWidthAdj, 1, 0);

    GtkObject* leftHeightAdj = 0;
    leftHeightAdj = gtk_adjustment_new(0, 0, height-1, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)leftYW, (GtkAdjustment*)leftHeightAdj, 1, 0);

    GtkObject* rightHeightAdj = 0;
    rightHeightAdj = gtk_adjustment_new(0, 0, height-1, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)rightYW, (GtkAdjustment*)rightHeightAdj, 1, 0);

    GtkObject* roiSizeAdj = 0;
    roiSizeAdj = gtk_adjustment_new(0, 0, 1000, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)ROIsizeW, (GtkAdjustment*)roiSizeAdj, 1, 0);

    GtkObject* thresholdValueAdj = 0;
    thresholdValueAdj = gtk_adjustment_new(0, 0, 1000, 1, 0, 0);
    gtk_spin_button_configure((GtkSpinButton*)thresholdValueW, (GtkAdjustment*)thresholdValueAdj, 1, 5);

    g_object_unref (G_OBJECT (builder));    
    gtk_main ();
        
    return 0;
}
