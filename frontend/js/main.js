
(function ($) {
    "use strict";

    function postData(url = '', data = {}) {
      // Default options are marked with *
        return fetch(url, {
            method: "POST", // *GET, POST, PUT, DELETE, etc.
            mode: "cors", // no-cors, cors, *same-origin
            cache: "no-cache", // *default, no-cache, reload, force-cache, only-if-cached
            credentials: "same-origin", // include, *same-origin, omit
            headers: {
                "Content-Type": "application/json; charset=utf-8",
            },
            redirect: "follow", // manual, *follow, error
            referrer: "no-referrer", // no-referrer, *client
            body: JSON.stringify(data), // body data type must match "Content-Type" header
        })
        .then(response => response.json()); // parses response to JSON
    }

    $('#num_server_vendors').on('change', () => {
        var num_server_vendors = $('#num_server_vendors').val();
        var html_cost_server = "";
        var html_max_num_serv_vendor = "";
        var html_monthly_elec_server_vendor = "";
        var i;
        for(i = 0; i < num_server_vendors; i++){
            html_cost_server += "<div class=\"wrap-input100\">\
            <span class=\"label-input100\">Koszt serwera producenta " + i +  ".</span>\
					<input class=\"input100\" type=\"number\" id=\"cost_server_vendor_" + i + "\" placeholder=\"100,200,300,...\">\
					<span class=\"focus-input100\"></span>\
				</div>";

            html_max_num_serv_vendor += "<div class=\"wrap-input100\">\
            <span class=\"label-input100\">Maksymalna liczba serwerów producenta " + i +  ".</span>\
					<input class=\"input100\" type=\"number\" id=\"max_num_server_vendor_" + i + "\" placeholder=\"100,200,300,...\">\
					<span class=\"focus-input100\"></span>\
				</div>";

            html_monthly_elec_server_vendor += "<div class=\"wrap-input100\">\
            <span class=\"label-input100\">Miesięczny pobór prądu przez serwer producenta " + i +  ".</span>\
					<input class=\"input100\" type=\"number\" id=\"monthly_elec_serv_vendor_" + i + "\" placeholder=\"100,200,300,...\">\
					<span class=\"focus-input100\"></span>\
				</div>";
        }

        $('#generated_server_cost').html(html_cost_server);
        $('#generated_max_num_serv_vendor').html(html_max_num_serv_vendor);
        $('#generated_monthly_elec_server_vendor').html(html_monthly_elec_server_vendor);
    });

    $('#num_rack_vendors').on('change', () => {
        var num_rack_vendors = $('#num_rack_vendors').val();
        var html_rack_cost = "";
        var html_max_num_rack_vendor = "";
        var html_rack_surface_vendor = "";
        var html_num_serv_in_rack_vendor = "";
        var i;
        for(i = 0; i < num_rack_vendors; i++){
            html_rack_cost += "<div class=\"wrap-input100\">\
            <span class=\"label-input100\">Koszt racka producenta " + i +  ".</span>\
					<input class=\"input100\" type=\"number\" id=\"cost_rack_vendor_" + i + "\" placeholder=\"100,200,300,...\">\
					<span class=\"focus-input100\"></span>\
				</div>";

            html_max_num_rack_vendor += "<div class=\"wrap-input100\">\
            <span class=\"label-input100\">Maksymalna liczba racków producenta " + i +  ".</span>\
					<input class=\"input100\" type=\"number\" id=\"max_num_rack_vendor_" + i + "\" placeholder=\"100,200,300,...\">\
					<span class=\"focus-input100\"></span>\
				</div>";

            html_rack_surface_vendor += "<div class=\"wrap-input100\">\
            <span class=\"label-input100\">Powierzchnia racka producenta " + i +  ".</span>\
					<input class=\"input100\" type=\"number\" id=\"rack_surface_vendor_" + i + "\" placeholder=\"100,200,300,...\">\
					<span class=\"focus-input100\"></span>\
				</div>";

            html_num_serv_in_rack_vendor += "<div class=\"wrap-input100\">\
            <span class=\"label-input100\">Ilość serwerów wchodzących do racka producenta " + i +  ".</span>\
					<input class=\"input100\" type=\"number\" id=\"num_serv_in_rack_vendor" + i + "\" placeholder=\"100,200,300,...\">\
					<span class=\"focus-input100\"></span>\
				</div>";
        }
        $('#generated_rack_cost').html(html_rack_cost);
        $('#generated_max_num_rack_vendor').html(html_max_num_rack_vendor);
        $('#generated_rack_m2_vendor').html(html_rack_surface_vendor);
        $('#generated_server_in_rack_vendor').html(html_num_serv_in_rack_vendor);
    });


    $("#submit").on('click', () => {

        var url = "";
        
        var m2_cost = $("#m2_cost").val();

        var num_server_vendors = $("#num_server_vendors").val();
        var cost_server = [];
        var max_num_serv_vendor = [];
        var monthly_elec_server_vendor = [];
        for(var i = 0; i < num_server_vendors; i++){
            cost_server.push($("#cost_server_vendor_" + i).val());
            max_num_serv_vendor.push($("#max_num_server_vendor_" + i).val());
            monthly_elec_server_vendor.push($("#monthly_elec_serv_vendor_" + i).val());
        }
        
        var num_rack_vendors = $("#num_rack_vendors").val();
        var rack_surface_vendor = [];
        var rack_cost_vendor = [];
        var rack_max_num_vendor = [];
        var num_serv_in_rack_vendor = [];
        for(var i = 0; i < num_rack_vendors; i++){
             rack_surface_vendor.push($("#rack_surface_vendor_" + i).val());
             rack_cost_vendor.push($("#cost_rack_vendor_" + i).val());
             rack_max_num_vendor.push($("#max_num_rack_vendor_" + i).val());
             num_serv_in_rack_vendor.push($("#num_serv_in_rack_vendor" + i).val());
        }

        var kWh_cost = $("#kWh_cost").val();
        var max_m2 = $("#max_m2").val();
        var min_all_servers = $("#min_all_servers").val();
        var min_num_racks = $("#min_num_racks").val();
        var max_kwh_usage = $("#max_kwh_usage").val();

        var form_data = {
           "m2_cost" :  m2_cost,
           "num_server_vendors" :  num_server_vendors,
           "cost_server" :  cost_server,
           "max_num_serv_vendor" :  max_num_serv_vendor,
           "monthly_elec_server_vendor" :  monthly_elec_server_vendor,
           "num_rack_vendors" :  num_rack_vendors,
           "rack_surface_vendor" :  rack_surface_vendor,
           "rack_cost_vendor" :  rack_cost_vendor,
           "rack_max_num_vendor" :  rack_max_num_vendor,
           "num_serv_in_rack_vendor" :  num_serv_in_rack_vendor,
           "kWh_cost" :  kWh_cost,
           "max_m2" :  max_m2,
           "min_all_servers" :  min_all_servers,
           "min_num_racks" :  min_num_racks,
           "max_kwh_usage" :  max_kwh_usage
        };

        console.log(JSON.stringify(form_data));
        postData(url, form_data)
          .then(data => console.log(JSON.stringify(data)))
          .catch(error => console.error(error));

    });
})(jQuery);
