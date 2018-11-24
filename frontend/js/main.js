
(function ($) {
    "use strict";


    /*==================================================================
    [ Focus Contact2 ]*/
    $('.input100').each(function(){
        $(this).on('blur', function(){
            if($(this).val().trim() != "") {
                $(this).addClass('has-val');
            }
            else {
                $(this).removeClass('has-val');
            }
        })    
    })
  
  
    /*==================================================================
    [ Validate ]*/
    var name = $('.validate-input input[name="name"]');
    var email = $('.validate-input input[name="email"]');
    var message = $('.validate-input textarea[name="message"]');


    $('.validate-form').on('submit',function(){
        var check = true;

        if($(name).val().trim() == ''){
            showValidate(name);
            check=false;
        }


        if($(email).val().trim().match(/^([a-zA-Z0-9_\-\.]+)@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.)|(([a-zA-Z0-9\-]+\.)+))([a-zA-Z]{1,5}|[0-9]{1,3})(\]?)$/) == null) {
            showValidate(email);
            check=false;
        }

        if($(message).val().trim() == ''){
            showValidate(message);
            check=false;
        }

        return check;
    });


    $('.validate-form .input100').each(function(){
        $(this).focus(function(){
           hideValidate(this);
       });
    });

    function showValidate(input) {
        var thisAlert = $(input).parent();

        $(thisAlert).addClass('alert-validate');
    }

    function hideValidate(input) {
        var thisAlert = $(input).parent();

        $(thisAlert).removeClass('alert-validate');
    }
    
    $("#submit").on('click', () => {
        var m2_cost = $("#m2_cost").val();
        var num_server_vendors = $("#num_server_vendors").val();
        var cost_server_vendor = $("#cost_server_vendor").val();
        var num_rack_vendors = $("#num_rack_vendors").val();
        var cost_rack_vendor = $("#cost_rack_vendor").val();
        var kWh_cost = $("#kWh_cost").val();
        var max_m2 = $("#max_m2").val();
        var min_all_servers = $("#min_all_servers").val();
        var max_vendor_servers = $("#max_vendor_servers").val();
        var min_num_racks = $("#min_num_racks").val();
        var max_vendor_racks = $("#max_vendor_racks").val();
        var max_kwh_usage = $("#max_kwh_usage").val();
        var surface_vendor_rack = $("#surface_vendor_rack").val();
        var cost_rack_vendor = $("#cost_rack_vendor").val();
        var amount_elec_server_vendor = $("#amount_elec_server_vendor").val();



    });   

})(jQuery);
