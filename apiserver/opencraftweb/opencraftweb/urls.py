from django.conf.urls import include,url
from django.contrib import admin
from django.conf import settings
from django.conf.urls.static import static

import views

urlpatterns = [
    url(r'^admin/', admin.site.urls),
    url(r'^api/',include('opencraft.urls')),
    url(r'', views.index, name='index')
] 
