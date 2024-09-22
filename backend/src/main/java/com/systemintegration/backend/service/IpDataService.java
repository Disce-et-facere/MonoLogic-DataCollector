package com.systemintegration.backend.service;

import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;
import org.springframework.web.util.UriComponentsBuilder;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.core.ParameterizedTypeReference;
import org.springframework.http.ResponseEntity;

import java.util.Map;

@Service
public class IpDataService {

    @Value("${IP_LOOKUP_APIKEY}")
    private String API_KEY;

    private final String IPDATA_URL = "https://api.ipdata.co/";

    public Map<String, Object> getIpLocation(String ip) {
        // System.out.println("Fetching location data for IP: " + ip);
        RestTemplate restTemplate = new RestTemplate();
        String url = UriComponentsBuilder.fromHttpUrl(IPDATA_URL + ip)
                .queryParam("api-key", API_KEY)
                .toUriString();

        ResponseEntity<Map<String, Object>> response = restTemplate.exchange(
                url,
                org.springframework.http.HttpMethod.GET,
                null,
                new ParameterizedTypeReference<Map<String, Object>>() {
                });
        return response.getBody();
    }
}
