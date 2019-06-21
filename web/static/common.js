$(document).ready(function(){
  /* init */
  $('body').css('height', window.innerHeight - 20).css('width', window.innerWidth - 20);
  $('#appParam').hide();
  $('#samplingRules').hide();
  $('div[group=std],div[group=api]').hide();
  $('pre').hide().first().show();
  $('#main-grid').css('left', $('#side').width()+16); 

  $('.compare-item').height(window.innerHeight * 0.7).first().addClass('active').show();
  $('.compare-item').css('bottom', window.innerHeight - $('#compareModal .actions').height() - $('#compreModal .actions').css('bottom'));

  $('#prev').click(function() {
    let active_item = $('.compare-item.active')[0];
    let length = $('.compare-item').length;
    let new_idx, idx = -1;
    for(let i=0; i < length; ++ i) {
      if($('.compare-item')[i] === active_item){
        idx = i;
        break;
      }
    }
    if(idx === 0) {
      new_idx = length - 1;
    }
    else {
      new_idx = idx - 1;
    }
    $('.compare-item.active').fadeOut(400,
      function() {
        $($('.compare-item')[new_idx]).fadeIn().addClass('active');
      }
    ).removeClass('active');
    console.log('prev now active image ', idx);
  });

  $('#next').click(function() {
    let active_item = $('.compare-item.active')[0];
    let length = $('.compare-item').length;
    let new_idx, idx = -1;
    for(let i=0; i < length; ++ i) {
      if($('.compare-item')[i] === active_item){
        idx = i;
        break;
      }
    }

    new_idx = (idx + 1) % length;
    $('.compare-item.active').fadeOut(400,
      function() {
        $($('.compare-item')[new_idx]).addClass('active').fadeIn();
      }
    ).removeClass('active');
    console.log('next now active image ', idx);
  });

  /* config */
  $('#apps').change(function(){
    var opt = $('#apps option:selected');
    var data_value = opt.attr('value');

    // app param
    if (data_value === "cd" ||
        data_value === "fco") {
      var title = opt.text() + " Parameters";
      $('#appParam .header:first-of-type').text(title);
      $('#appParam .field.param').hide();
      var selector_str = '#appParam .' + data_value;
      $(selector_str).show();
      $('#appParam').fadeIn();
    } else {
      $('#appParam').fadeOut();
    }

    // sampling rules
    if (data_value === "tc" ||
        data_value === "gm" ||
        data_value === "cd" ||
        data_value === "fco") {
      $('#samplingRules .field.rules').hide();
      var selector_str = '#samplingRules .' + data_value;
      $(selector_str).show();
      $('#samplingRules').fadeIn();
    } else {
      $('#samplingRules').fadeOut();
    }

    // dataset
    if(data_value === "cd" || data_value=="fco"){
      $('#dataset option[value="friendster"]').hide();
      if(data_value === "fco"){
        $('#dataset option[value="tencent"]').hide();
        $('#dataset option[value="dblp"]').show();
      }
      else{
        $('#dataset option[value="tencent"]').show();
        $('#dataset option[value="dblp"]').hide();
      }
    }
    else{
      $('#dataset option[value="friendster"]').show();
      if (data_value === ""){
        $('#dataset option[value="dblp"]').show();
        $('#dataset option[value="tencent"]').show();
      }
      else{
        $('#dataset option[value="dblp"]').hide();
        $('#dataset option[value="tencent"]').hide();
      }
    }
  });
  $('#dataset').change(function() {
    var opt = $('#dataset option:selected');
    var data_value = opt.attr('value');
    $('#apps option').show();
    if(data_value === "friendster"){
      $('#apps option[value="cd"]').hide();
      $('#apps option[value="fco"]').hide();
    }
    else if(data_value === "dblp" || data_value === "tencent") {
      $('#apps option[value="tc"]').hide();
      $('#apps option[value="mc"]').hide();
      $('#apps option[value="gm"]').hide();
      if(data_value === "dblp"){
        $('#apps option[value="cd"]').hide();
      }
      else if(data_value === "tencent")
        $('#apps option[value="fco"]').hide();
    }
  });
  /* code */
  var radio_checked = $('#apiradios input[type=radio]:first-of-type');
  var changeCodes = function(now){
    let value = radio_checked.attr('value');
    let new_value = now.attr('value');
    let selector = 'pre[value='+value+']';
    let new_selector = 'pre[value='+new_value+']';
    console.log(selector, new_selector);
    if (new_value != value){
      $(selector).fadeOut(300, function() {
        $(new_selector).fadeIn();
      });
    }
    radio_checked = now;
  }
  $('#apiradios input').click(function(){changeCodes($(this));});


  $('#aboutOpen').click(
    function(){
      $('#aboutModal').modal('show');
    });
  $('#configOpen').click(
    function(){
      $('#configModal').modal('show');
      $('.popup').popup(
        {on: 'click'}
      );
    });
  $('#aboutOpen').click(
    function(){
      $('#aboutModal').modal('show');
    });
  $('#compareOpen').click(
    function(){
      $('#compareModal').modal('show');
    });
  $('#teamOpen').click(
    function(){
      $('#teamModal').modal('show');
    }
  );

  /* content block */
  $('#contentMenu>.item').click(function(){
    if($(this).hasClass('active')){
      return;
    }
    // reset past active
    let orig_active = $('#contentMenu>.item.active');
    let group = orig_active.attr('group');
    let selector = '#console [group=' + group + ']';
    $(selector).hide();
    orig_active.removeClass('active');

    // activate this item
    group = $(this).attr('group');
    selector = '#console [group=' + group + ']';
    $(selector).show();
    $(this).addClass('active');
  });
});
