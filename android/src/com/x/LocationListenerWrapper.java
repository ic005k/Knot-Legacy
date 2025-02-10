package com.x;

import android.content.Context;

import android.location.Location;

import android.location.LocationListener;

import android.location.LocationManager;

import android.os.Bundle;

public class LocationListenerWrapper implements LocationListener {

    private static final long MIN_TIME_BETWEEN_UPDATES = 5000; // 5秒

    private static final float MIN_DISTANCE_CHANGE_FOR_UPDATES = 1; // 1米

    private LocationManager locationManager;

    private Location lastLocation;

    private double totalDistance = 0;
    private double latitude = 0;
    private double longitude = 0;

    public LocationListenerWrapper(Context context) {

        locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);

        try {

            locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER,

                    MIN_TIME_BETWEEN_UPDATES,

                    MIN_DISTANCE_CHANGE_FOR_UPDATES,

                    this);

        } catch (SecurityException e) {

            e.printStackTrace();

        }

    }

    @Override

    public void onLocationChanged(Location location) {

        if (lastLocation != null) {

            totalDistance += lastLocation.distanceTo(location);
            // 获取经纬度
            latitude = location.getLatitude();
            longitude = location.getLongitude();

        }

        lastLocation = location;

    }

    @Override

    public void onStatusChanged(String provider, int status, Bundle extras) {
    }

    @Override

    public void onProviderEnabled(String provider) {
    }

    @Override

    public void onProviderDisabled(String provider) {
    }

    public double getTotalDistance() {

        return totalDistance;

    }

    public double getLatitude() {

        return latitude;

    }

    public double getLongitude() {

        return longitude;

    }

    // 停止 GPS 更新
    public double stopGpsUpdates() {
        if (locationManager != null) {
            try {
                locationManager.removeUpdates(this);
            } catch (SecurityException e) {
                e.printStackTrace();
            }
        }
        return totalDistance;
    }

}
