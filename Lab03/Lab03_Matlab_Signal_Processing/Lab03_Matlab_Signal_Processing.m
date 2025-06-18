clc, clearvars, close all

%% Lab03: Generation, Processing, and Analysis of Sine Waves

%% 1) Read Data
% Load the provided signal 'signal.csv' using the 'csvread()' function and
% split the signal and time into two separate vectors.
data =readmatrix('signal.csv');
fprintf('1) Start.\n')

signal= data(:,2);
time= data(:,1);


% Plot the signal
figure(1)
plot(time,signal);
xlabel('time [s]');
ylabel('signal');
xlim([0 max(time)]);
grid on
hold on



fprintf('1) Done.\n')

%% 2/3) Butterworth Filter
% Design a Butterworth filter that fulfills the required characteristics 
% given in the assignment description. Use the built-in functions of
% Matlab. The 'doc' and 'help' functions might be useful to obtain detailed
% information.



fprintf('2) Start.\n')

% designing a Butterworth digital filter
Wp= 30/2500; %pass band freq
Ws= 200/2500; %stop band freq
Ap= 3; %pass band attenuation
As= 30; %stopband attenuation

[n, Wn] = buttord(Wp, Ws, Ap, As);
disp(n);
disp(Wn);

fprintf('2) Done.\n')

% 3) Calculate the filter coefficients and apply them to the signal, i.e.,
% filter the signal. Plot the filtered signal into the same figure as the
% original signal. Make sure to add a title, the axis descriptions, and a
% legend.


fprintf('3) Start.\n')

[b, a] = butter(n, Wn); 
filtered_signal = filter(b, a, signal);

plot(time,filtered_signal);  % the shift is because of the order of the filter, increasing the order will increase the phase shift
legend('Raw Signal', 'Filtered Signal');

fprintf('3) Done.\n')

%% 4. Fourier Transform
% Calculate the single-sided Fourier transform of the filtered signal.

fprintf('4) Start.\n')

% 4.1) First, obtain the length of the original and filtered signal and 
% calculate their means. Print both mean values.
l_o= length(signal);
l_f= length(filtered_signal);
mean_original =mean(signal);
mean_filtered= mean(filtered_signal);
disp(mean_original);
disp(mean_filtered);


% 4.2) Do the FFT for both signals; see the docs and lecture slides for
% help. Make sure to remove the mean from the signals. THIS IS VERY
% IMPORTANT BECAUESE IT WILL HIDE THE IMPORTANT INFO AND WE WILL ONLY SEE A
% PEACK AT f= 0 Hz




FT_signal = fftshift(abs(fft(signal-mean_original)));


FT_filtered = fftshift(abs(fft(filtered_signal-mean_filtered)));



% 4.2.3) When plotting, only visualize the spectrum of to 500 Hz.

Fs =5000; %sampling freq
N = length(signal);  % Number of samples
f = (-N/2:N/2-1)*(Fs/N);  % Frequency axis in Hz




figure(2)
plot(f,FT_signal);
xlabel('frequency');
xlim([-250 250]);
ylabel('FFT');
grid on
hold on
plot(f,FT_filtered);
legend('Raw Signal', 'Filtered Signal');



fprintf('4) Done.\n')

%% 5. Frequency Identification
% Write a function that automatically detects a signals frequency based
% on its frequency spectrum. You can assume there's only a single signal
% and noise has been removed. The function must return the amplitude and
% the frequency of this signal.

fprintf('5) Start.\n')

% 5.2) What is the frequency of they signal you have analyzed?
%sig_raw= signal- mean_original;
sig_clean= filtered_signal-mean_filtered;

[a, f]= detect_signal_freq(sig_clean,Fs);
disp(a);
disp(f);


fprintf('5) Done.\n')

function [peak_amp, peak_freq] =detect_signal_freq(signal, f_sampling)
  
  fft_signal = fft(signal(1:end-1)); % odd sized signal: (1:end-1)
  amplitude = abs(fft_signal/length(fft_signal));
  window = amplitude(1:length(fft_signal)/2+1); %only positive frequencies
  window(2:end-1) = 2*window(2:end-1);
  frequencies = f_sampling*(0:(length(fft_signal)/2))/length(fft_signal);


  [peak_amp, idx] = max(window(2:end));
  idx = idx + 1;
  peak_freq = frequencies(idx);

 

  figure(3)
  plot(frequencies, window);
  xlabel('frequency [Hz]');
  ylabel('Aplitude');
  grid on

    
end
 
