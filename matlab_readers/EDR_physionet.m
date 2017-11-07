run('Reader_ECG_SD.m')

keepvars = {'rec_ecg','ecg_time','ecg_filt'};
clearvars('-except',keepvars{:});
fs = 400;
one_mv = 1.045792488262896e+04;
rec_ecg = rec_ecg/one_mv;   ecg_filt = ecg_filt/one_mv;

[qrs_amp_raw,qrs_i_raw,delay]=pan_tompkin(rec_ecg,fs,0);
qrs_time = qrs_i_raw/fs;
cardio_freq = 60./diff(qrs_time);
close all;



y = edr(0,rec_ecg,qrs_time,400);%,pqoff, jpoff, gain_ecg, channel, show)
y2 = reshape(y,[],2);

[pks,locs] = findpeaks(y2(:,2));


plot(ecg_time,ecg_filt); hold all; plot(qrs_time,qrs_amp_raw,'o');
figure; plot(y2(:,1),y2(:,2)); hold on;  plot(y2(locs,1),pks,'o');
t_btw_cycles = diff(y2(locs,1));
freq = 60./t_btw_cycles;
figure; plot(freq)