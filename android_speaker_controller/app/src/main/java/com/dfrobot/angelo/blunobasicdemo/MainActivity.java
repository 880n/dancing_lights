package com.dfrobot.angelo.blunobasicdemo;

import android.content.Context;
import android.os.Bundle;
import android.content.Intent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ScrollView;
import android.widget.TextView;
import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.provider.Settings;
import android.speech.RecognitionListener;
import android.speech.RecognizerIntent;
import android.speech.SpeechRecognizer;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.EditText;

import java.util.ArrayList;
import java.util.Locale;

public class MainActivity  extends BlunoLibrary {
	private Button buttonScan;
	private Button buttonSerialSend;
	private EditText serialSendText;
	private TextView serialReceivedText;
	private Button mode1;
	private Button mode2;
	private Button brightness1;
	private Button brightness2;
	private Button up;
	private Button down;
	private Button left;
	private Button right;
	private Button snake;
	private Button dancing;
	private ImageButton home;
	private ImageButton mic;
	private String speechtext;


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		onCreateProcess();														//onCreate Process by BlunoLibrary
		serialBegin(115200);													//set the Uart Baudrate on BLE chip to 115200

		serialReceivedText=(TextView) findViewById(R.id.serialReveicedText);	//initial the EditText of the received data
		serialSendText=(EditText) findViewById(R.id.serialSendText);			//initial the EditText of the sending data

		checkPermission();

		final EditText editText = (EditText)findViewById(R.id.editText);

		final SpeechRecognizer mSpeechRecognizer = SpeechRecognizer.createSpeechRecognizer(this);


		final Intent mSpeechRecognizerIntent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
		mSpeechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
				RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
		mSpeechRecognizerIntent.putExtra(RecognizerIntent.EXTRA_LANGUAGE,
				Locale.getDefault());


		mSpeechRecognizer.setRecognitionListener(new RecognitionListener() {
			@Override
			public void onReadyForSpeech(Bundle bundle) {

			}

			@Override
			public void onBeginningOfSpeech() {

			}

			@Override
			public void onRmsChanged(float v) {

			}

			@Override
			public void onBufferReceived(byte[] bytes) {

			}

			@Override
			public void onEndOfSpeech() {

			}

			@Override
			public void onError(int i) {

			}

			@Override
			public void onResults(Bundle bundle) {
				//getting all the matches
				ArrayList<String> matches = bundle
						.getStringArrayList(SpeechRecognizer.RESULTS_RECOGNITION);

				//displaying the first match
				if (matches != null)
					editText.setText(matches.get(0));
					speechtext=matches.get(0);
				if(speechtext!=null){
					if(speechtext.contains("brightness")){
						if(speechtext.contains("increase")) {
							String s = "b";
							serialSend(s);
						}
						if(speechtext.contains("decrease")) {
							String s = "B";
							serialSend(s);
						}
					}
					if(speechtext.contains("next")){
							String s = "m";
							serialSend(s);
					}
					if(speechtext.contains("back")){
						String s = "M";
						serialSend(s);
					}
					if(speechtext.contains("dancing")){
						String s="d";
						serialSend(s);

					}
					if(speechtext.contains("snake")){
						String s="s";
						serialSend(s);

					}
					if(speechtext.contains("home")){
						String s="h";
						serialSend(s);

					}
				}
			}

			@Override
			public void onPartialResults(Bundle bundle) {

			}

			@Override
			public void onEvent(int i, Bundle bundle) {

			}
		});

		findViewById(R.id.mic).setOnTouchListener(new View.OnTouchListener() {
			@Override
			public boolean onTouch(View view, MotionEvent motionEvent) {
				switch (motionEvent.getAction()) {
					case MotionEvent.ACTION_UP:
						mSpeechRecognizer.stopListening();
						editText.setHint("You will see input here");
						break;

					case MotionEvent.ACTION_DOWN:
						mSpeechRecognizer.startListening(mSpeechRecognizerIntent);
						editText.setText("");
						editText.setHint("Listening...");
						break;
				}
				return false;
			}
		});

		buttonSerialSend = (Button) findViewById(R.id.buttonSerialSend);		//initial the button for sending the data
		buttonSerialSend.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub

				serialSend(serialSendText.getText().toString());				//send the data to the BLUNO
			}
		});

		buttonScan = (Button) findViewById(R.id.buttonScan);					//initial the button for scanning the BLE device
		buttonScan.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub

				buttonScanOnClickProcess();										//Alert Dialog for selecting the BLE device
			}
		});
		dancing = (Button) findViewById(R.id.dancing);		//initial the button for sending the data
		dancing.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="d";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		mode1 = (Button) findViewById(R.id.mode1);		//initial the button for sending the data
		mode1.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="m";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		mode2 = (Button) findViewById(R.id.mode2);		//initial the button for sending the data
		mode2.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="M";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		brightness1 = (Button) findViewById(R.id.brightness1);		//initial the button for sending the data
		brightness1.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="b";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		brightness2 = (Button) findViewById(R.id.brightness2);		//initial the button for sending the data
		brightness2.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="B";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		home=(ImageButton) findViewById(R.id.home);
		home.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="h";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		snake = (Button) findViewById(R.id.snake);		//initial the button for sending the data
		snake.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="s";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		up = (Button) findViewById(R.id.up);		//initial the button for sending the data
		up.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="2";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		down = (Button) findViewById(R.id.down);		//initial the button for sending the data
		down.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="8";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		right = (Button) findViewById(R.id.right);		//initial the button for sending the data
		right.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="4";
				serialSend(s);				//send the data to the BLUNO
			}
		});

		left = (Button) findViewById(R.id.left);		//initial the button for sending the data
		left.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				String s="6";
				serialSend(s);				//send the data to the BLUNO
			}
		});
	}

	protected void onResume(){
		super.onResume();
		System.out.println("BlUNOActivity onResume");
		onResumeProcess();														//onResume Process by BlunoLibrary
	}



	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		onActivityResultProcess(requestCode, resultCode, data);					//onActivityResult Process by BlunoLibrary
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	protected void onPause() {
		super.onPause();
		onPauseProcess();														//onPause Process by BlunoLibrary
	}

	protected void onStop() {
		super.onStop();
		onStopProcess();														//onStop Process by BlunoLibrary
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		onDestroyProcess();														//onDestroy Process by BlunoLibrary
	}

	@Override
	public void onConectionStateChange(connectionStateEnum theConnectionState) {//Once connection state changes, this function will be called
		switch (theConnectionState) {											//Four connection state
			case isConnected:
				buttonScan.setText("Connected");
				break;
			case isConnecting:
				buttonScan.setText("Connecting");
				break;
			case isToScan:
				buttonScan.setText("Scan");
				break;
			case isScanning:
				buttonScan.setText("Scanning");
				break;
			case isDisconnecting:
				buttonScan.setText("isDisconnecting");
				break;
			default:
				break;
		}
	}

	@Override
	public void onSerialReceived(String theString) {							//Once connection data received, this function will be called
		// TODO Auto-generated method stub
		serialReceivedText.append(theString);							//append the text into the EditText
		//The Serial data from the BLUNO may be sub-packaged, so using a buffer to hold the String is a good choice.
		((ScrollView)serialReceivedText.getParent()).fullScroll(View.FOCUS_DOWN);
	}

	private void checkPermission() {
			if (!(ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) == PackageManager.PERMISSION_GRANTED)) {
				Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS,
						Uri.parse("package:" + getPackageName()));
				startActivity(intent);
				finish();

		}
	}

}