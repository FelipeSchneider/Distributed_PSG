%% reading the signal
run('Reader_ECG_SD.m')

keepvars = {'rec_ecg','ecg_time','ecg_filt'};
clearvars('-except',keepvars{:});
fs = 400;
one_mv = 1.045792488262896e+04;
rec_ecg_mv = rec_ecg/one_mv;   ecg_filt = ecg_filt/one_mv;

%% QRS detection
[qrs_amp_raw,qrs_i_raw,delay]=pan_tompkin(rec_ecg_mv,fs,0);
qrs_time = qrs_i_raw/fs;
cardio_freq = 60./diff(qrs_time);
close all;

%% EDR extraction
y = edr(0,rec_ecg_mv,qrs_time,400);%,pqoff, jpoff, gain_ecg, channel, show)
y2 = reshape(y,[],2);
edr = y2(:,2);
t_edr = (y2(:,1));


%% Respiration frequency calculation
[pks,locs] = findpeaks(y2(:,2));
t_btw_cycles = diff(y2(locs,1));
freq = 60./t_btw_cycles;

interpoled_edr = [];
for i=1:length(edr)-1
    interpoled_edr = [interpoled_edr linspace(edr(i),edr(i+1),(t_edr(i+1)-t_edr(i))*400)];
end
t_edr = 1/400:1/400:length(interpoled_edr)/400;

j=1;
inc = 12;
tre = 120;
for i=1:inc*fs:length(interpoled_edr)-tre*fs
    [fft_EDR(:,j),f] = periodogram(interpoled_edr(i:tre*fs+i),[],[],fs);
    [M,I] = max(fft_EDR(5:end,j));
    freq_resp(i:i+inc*fs-1) = ones(1,inc*fs).*f(I+4)*60;
    relacao_freq(i:i+inc*fs-1) = ones(1,inc*fs).*(sum(fft_EDR(5:20,j)))/(sum(fft_EDR(20:50,j)));
    j=j+1;
end
%% plots
t_freq = 1/400:1/400:length(freq_resp)/400;

figure; plot(ecg_time,ecg_filt); hold all; plot(qrs_time,qrs_amp_raw,'o');
title('ECG and QRS detection'); legend('ECG signal','QRS detect');
xlabel('Time (sec.)'); ylabel('milivolts');
figure; plot(y2(:,1),y2(:,2)); hold on;  plot(y2(locs,1),pks,'o');
title('EDR signal'); legend('EDR signal','EDR peaks');
xlabel('Time (sec.)'); ylabel('EDR');
figure; plot(y2(locs(1:length(freq)),1),freq); hold on; plot(t_freq,freq_resp);
title('Respiratory Frequency'); legend('Peak detection method','FFT method');
xlabel('Time (sec.)'); ylabel('Resp. Freq. (cpm)');
xlabel('Time (sec.)');